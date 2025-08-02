#include <systems/helper/player.hpp>

#include <print>

#include <components/entity.hpp>
#include <components/tower.hpp>
#include <components/weapon.hpp>
#include <components/navigation.hpp>
#include <components/map.hpp>
#include <components/player.hpp>

#include <systems/helper/resource.hpp>
#include <systems/helper/tower.hpp>
#include <systems/helper/weapon.hpp>

#include <entt/entt.hpp>

namespace systems::helper
{
	auto Player::try_build_tower(entt::registry& registry, const sf::Vector2f world_position) noexcept -> bool
	{
		using namespace components;

		auto& [map] = registry.ctx().get<map_ex::Map>();
		const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();

		const auto& [player_selected_tower_type, player_selected_weapon_type] = registry.ctx().get<player::Interaction>();
		auto& [player_towers] = registry.ctx().get<player::Tower>();

		auto& [flow_field] = registry.ctx().get<navigation::FlowField>();
		auto& [cache_paths] = registry.ctx().get<navigation::Path>();

		const auto grid_position = map.coordinate_world_to_grid(world_position);

		// 检查是否选择了塔
		if (player_selected_tower_type.type == EntityType::invalid_type)
		{
			std::println("未选择塔类型,建造失败");
			return false;
		}

		// 检查位置是否合法
		if (not map.inside(grid_position.x, grid_position.y) or map.at(grid_position.x, grid_position.y) != map::TileType::BUILDABLE_FLOOR)
		{
			std::println("位置({}:{})不在地图内或不可建造", grid_position.x, grid_position.y);
			return false;
		}

		// 检查资源是否足够
		if (not Resource::require(registry, player_selected_tower_type))
		{
			std::println("资源不足");
			return false;
		}

		// 检查路径是否会被堵死
		std::vector<std::reference_wrapper<map::path_type>> changed_cache_path{};
		{
			map.set(grid_position.x, grid_position.y, map::TileType::TOWER);

			// 只要有一条路线被堵死就不允许建造
			for (auto& cache_path: cache_paths)
			{
				// 建造位置不在路径上不会影响路径
				if (not std::ranges::contains(cache_path, grid_position))
				{
					continue;
				}

				if (not map::PathFinder::is_reachable(
					map,
					cache_path.front(),
					end_gates
				))
				{
					// 该起点找不到一条达到任意终点的路径
					map.set(grid_position.x, grid_position.y, map::TileType::BUILDABLE_FLOOR);

					std::println("在({}:{})建造塔后将导致至少一个起点无法到达任意终点", grid_position.x, grid_position.y);
					return false;
				}

				changed_cache_path.emplace_back(std::ref(cache_path));
			}
		}

		// 建造
		const auto entity = Tower::build(registry, grid_position, player_selected_tower_type);
		if (entity == entt::null)
		{
			std::println("建造失败");
			return false;
		}

		// 消耗资源
		Resource::consume_unchecked(registry, player_selected_tower_type);

		// 记录建造的塔
		player_towers.emplace(grid_position, entity);

		// 更新流场
		// todo: 即使建造位置不在路径上,流场更新也可能造成某些路径变化
		// 如此会造成实际路径与显示路径不一致
		if (changed_cache_path.empty())
		{
			// 新进路径不变,仅是部分区域流向变动
			// 这里可以后台更新流场(不阻塞)
			flow_field.update(grid_position);
		}
		else
		{
			// 路径有变,必须等待流场更新完毕才能确定行进路径
			flow_field.update(grid_position);

			std::ranges::for_each(
				changed_cache_path,
				[&](map::path_type& cache_path) noexcept -> void
				{
					auto new_path = flow_field.path_of(cache_path.front(), std::numeric_limits<std::size_t>::max());
					assert(new_path.has_value());

					cache_path = *std::move(new_path);
				}
			);
		}

		return true;
	}

	auto Player::try_destroy_tower(entt::registry& registry, const sf::Vector2f world_position) noexcept -> bool
	{
		using namespace components;

		auto& [map] = registry.ctx().get<map_ex::Map>();
		auto& map_counter = registry.ctx().get<map_ex::Counter>();

		auto& [player_towers] = registry.ctx().get<player::Tower>();

		auto& [flow_field] = registry.ctx().get<navigation::FlowField>();

		const auto grid_position = map.coordinate_world_to_grid(world_position);

		// 检查位置是否合法
		if (not map.inside(grid_position.x, grid_position.y) or map.at(grid_position.x, grid_position.y) != map::TileType::TOWER)
		{
			std::println("位置({}:{})不在地图内或未建造塔", grid_position.x, grid_position.y);
			return false;
		}

		const auto it = player_towers.find(grid_position);
		assert(it != player_towers.end());

		const auto& [weapons] = registry.get<tower::Equipment>(it->second);

		// 返还资源
		{
			// 武器
			if (not weapons.empty())
			{
				std::vector<EntityType> types{};
				types.reserve(weapons.size());

				std::ranges::transform(
					weapons,
					std::back_inserter(types),
					[&](const entt::entity entity) noexcept -> EntityType
					{
						return registry.get<const EntityType>(entity);
					}
				);

				Resource::acquire(registry, types);
			}

			// 塔
			const auto tower_type = registry.get<const EntityType>(it->second);
			Resource::acquire(registry, tower_type);
		}

		// 销毁塔
		{
			if (not weapons.empty())
			{
				registry.destroy(weapons.begin(), weapons.end());
			}
			registry.destroy(it->second);
			player_towers.erase(it);
		}

		// 设置地块
		{
			map.set(grid_position.x, grid_position.y, map::TileType::BUILDABLE_FLOOR);
			map_counter.built_tower -= 1;
		}

		// 更新流场
		flow_field.update(grid_position);

		return true;
	}

	auto Player::try_equip_weapon(entt::registry& registry, const sf::Vector2f world_position) noexcept -> bool
	{
		using namespace components;

		const auto& [map] = registry.ctx().get<const map_ex::Map>();

		const auto& [player_selected_tower_type, player_selected_weapon_type] = registry.ctx().get<const player::Interaction>();
		auto& [player_towers] = registry.ctx().get<player::Tower>();

		const auto grid_position = map.coordinate_world_to_grid(world_position);

		// 检查是否选择了武器
		if (player_selected_weapon_type.type == EntityType::invalid_type)
		{
			std::println("未选择武器类型,装备失败");
			return false;
		}

		// 检查选择地块是否有塔
		if (not map.inside(grid_position.x, grid_position.y) or map.at(grid_position.x, grid_position.y) != map::TileType::TOWER)
		{
			std::println("位置({}:{})不在地图内或未建造塔", grid_position.x, grid_position.y);
			return false;
		}

		// 检查资源是否足够
		if (not Resource::require(registry, player_selected_weapon_type))
		{
			std::println("资源不足");
			return false;
		}

		const auto it = player_towers.find(grid_position);
		assert(it != player_towers.end());

		// 安装武器
		const auto entity = Weapon::equip(registry, it->second, player_selected_weapon_type);
		assert(entity != entt::null);

		// 消耗资源
		Resource::consume_unchecked(registry, player_selected_weapon_type);

		return true;
	}

	auto Player::try_remove_weapon(entt::registry& registry, const sf::Vector2f world_position) noexcept -> bool
	{
		using namespace components;

		const auto& [map] = registry.ctx().get<const map_ex::Map>();

		auto& [player_towers] = registry.ctx().get<player::Tower>();

		const auto grid_position = map.coordinate_world_to_grid(world_position);

		// 检查选择地块是否有塔
		if (not map.inside(grid_position.x, grid_position.y) or map.at(grid_position.x, grid_position.y) != map::TileType::TOWER)
		{
			std::println("位置({}:{})不在地图内或未建造塔", grid_position.x, grid_position.y);
			return false;
		}

		const auto it = player_towers.find(grid_position);
		assert(it != player_towers.end());

		// todo: 如何优雅地选择要解除的武器?
		auto& [weapons] = registry.get<tower::Equipment>(it->second);
		const auto weapon_to_remove = weapons.back();
		weapons.pop_back();

		// 返还资源
		{
			const auto type = registry.get<EntityType>(weapon_to_remove);

			Resource::acquire(registry, type);
		}

		// 销毁武器
		{
			registry.destroy(weapon_to_remove);
		}

		return true;
	}
}
