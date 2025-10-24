#include <helper/player.hpp>

#include <algorithm>
#include <ranges>
#include <print>

#include <components/core/tags.hpp>
#include <components/core/transform.hpp>
#include <components/combat/unit.hpp>
#include <components/game/player.hpp>
#include <components/map/map.hpp>
#include <components/map/navigation.hpp>

#include <helper/resource.hpp>
#include <helper/tower.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Player::try_build_tower(entt::registry& registry, const sf::Vector2f position) noexcept -> bool
	{
		using namespace components;

		auto& [tile_map] = registry.ctx().get<map_ex::TileMap>();
		const auto& [end_gates] = registry.ctx().get<const map_ex::EndGate>();

		auto& [flow_field] = registry.ctx().get<navigation::FlowField>();
		auto& [cache_paths] = registry.ctx().get<navigation::Path>();

		const auto& [player_selected_tower_type] = registry.ctx().get<const player::Interaction>();
		auto& [player_tower] = registry.ctx().get<player::Tower>();

		const auto grid_position = tile_map.coordinate_world_to_grid(position);

		// 检查是否选择了塔
		if (player_selected_tower_type == combat::invalid_type)
		{
			std::println("未选择塔类型,建造失败");
			return false;
		}

		// 检查位置是否合法
		if (not tile_map.inside(grid_position.x, grid_position.y) or tile_map.at(grid_position.x, grid_position.y) != map::TileType::BUILDABLE_FLOOR)
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
			tile_map.set(grid_position.x, grid_position.y, map::TileType::TOWER);

			// 只要有一条路线被堵死就不允许建造
			for (auto& cache_path: cache_paths)
			{
				// 建造位置不在路径上不会影响路径
				if (not std::ranges::contains(cache_path, grid_position))
				{
					continue;
				}

				if (not map::PathFinder::is_reachable(
					tile_map,
					cache_path.front(),
					end_gates
				))
				{
					// 该起点找不到一条到任意终点的路径
					tile_map.set(grid_position.x, grid_position.y, map::TileType::BUILDABLE_FLOOR);
					std::println("在({}:{})建造塔后将导致至少一个起点无法到达任意终点", grid_position.x, grid_position.y);
					return false;
				}

				changed_cache_path.emplace_back(std::ref(cache_path));
			}
		}

		// 构建塔实体
		const auto tower_entity = Tower::build(registry, player_selected_tower_type);
		if (tower_entity == entt::null)
		{
			std::println("建造塔失败");
			return false;
		}
		// 设置塔位置
		{
			// 确保位置位于网格中心
			const auto centered_position = tile_map.coordinate_grid_to_world(grid_position);
			registry.emplace<transform::Position>(tower_entity, centered_position);
		}
		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::tower>(tower_entity);

		// 消耗资源
		Resource::consume_unchecked(registry, player_selected_tower_type);
		// 记录建造的塔
		player_tower.emplace(grid_position, tower_entity);

		// 更新流场
		// todo: 看起来即使建造位置不在路径上,流场更新也可能造成某些路径变化
		// 如此会造成实际路径与显示路径不一致
		if (changed_cache_path.empty())
		{
			// 行进路径不变,仅是部分区域流向变动
			// 这里可以后台更新流场(不阻塞)
			flow_field.update(grid_position);
		}
		else
		{
			// 需要等待流场更新完毕才能确定行进路径
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

	auto Player::try_destroy_tower(entt::registry& registry, const sf::Vector2f position) noexcept -> bool
	{
		using namespace components;

		auto& [tile_map] = registry.ctx().get<map_ex::TileMap>();

		auto& [flow_field] = registry.ctx().get<navigation::FlowField>();

		auto& [player_tower] = registry.ctx().get<player::Tower>();

		const auto grid_position = tile_map.coordinate_world_to_grid(position);

		// 检查位置是否合法
		if (not tile_map.inside(grid_position.x, grid_position.y) or tile_map.at(grid_position.x, grid_position.y) != map::TileType::TOWER)
		{
			std::println("位置({}:{})不在地图内或未建造塔", grid_position.x, grid_position.y);
			return false;
		}

		const auto tower_it = player_tower.find(grid_position);
		if (tower_it == player_tower.end())
		{
			std::println("位置({}:{})的塔非你建造", grid_position.x, grid_position.y);
			return false;
		}

		// 返还资源
		Resource::acquire(registry, tower_it->second);

		// 销毁塔
		registry.destroy(tower_it->second);
		player_tower.erase(tower_it);

		// 设置地块
		tile_map.set(grid_position.x, grid_position.y, map::TileType::BUILDABLE_FLOOR);

		// 更新流场
		flow_field.update(grid_position);

		return true;
	}
}
