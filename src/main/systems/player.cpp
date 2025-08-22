#include <systems/player.hpp>

#include <algorithm>
#include <ranges>
#include <print>

#include <components/tags.hpp>
#include <components/player.hpp>
#include <components/map.hpp>
#include <components/navigation.hpp>

#include <systems/tower.hpp>
#include <systems/resource.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Player::initialize(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		sf::RectangleShape cursor_shape{{static_cast<float>(tile_map.tile_width()), static_cast<float>(tile_map.tile_height())}};

		cursor_shape.setOrigin(cursor_shape.getSize() / 2.f);
		cursor_shape.setFillColor(sf::Color::Transparent);
		cursor_shape.setOutlineColor(sf::Color::Green);
		cursor_shape.setOutlineThickness(2.f);

		registry.ctx().emplace<player::Interaction>(entity::invalid_type);
		registry.ctx().emplace<player::Tower>();
		registry.ctx().emplace<player::Resource>();
		registry.ctx().emplace<player::Statistics>(player::Statistics::size_type{0});
		registry.ctx().emplace<player::Render>(std::move(cursor_shape));
	}

	auto Player::update(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [player_resource] = registry.ctx().get<player::Resource>();
		auto& player_health = player_resource[resource::Type::HEALTH];

		// 到达终点的敌人
		const auto enemy_reached_view = registry.view<tags::enemy_reached>();
		const auto reached_count = enemy_reached_view.size();

		// todo: 每个敌人扣多少生命值?
		if (const auto cost_health = static_cast<resource::size_type>(reached_count) * 1;
			cost_health >= player_health)
		{
			player_health = 0;

			// todo: 游戏结束
		}
		else
		{
			player_health -= cost_health;
		}
	}

	auto Player::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		// 绘制游标方框
		{
			const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

			auto& [cursor] = registry.ctx().get<player::Render>();

			const auto mouse_position = sf::Mouse::getPosition(window);
			const auto mouse_grid_position = tile_map.coordinate_world_to_grid(mouse_position);

			if (tile_map.inside(mouse_grid_position.x, mouse_grid_position.y))
			{
				const auto world_position = tile_map.coordinate_grid_to_world(mouse_grid_position);
				cursor.setPosition(world_position);

				window.draw(cursor);
			}
		}
	}

	auto Player::try_build_tower(entt::registry& registry, const sf::Vector2f position) noexcept -> bool
	{
		using namespace components;

		auto& [tile_map] = registry.ctx().get<map_ex::TileMap>();
		const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();

		auto& [flow_field] = registry.ctx().get<navigation::FlowField>();
		auto& [cache_paths] = registry.ctx().get<navigation::Path>();

		const auto& [player_selected_tower_type] = registry.ctx().get<const player::Interaction>();
		auto& [player_tower] = registry.ctx().get<player::Tower>();

		const auto grid_position = tile_map.coordinate_world_to_grid(position);

		// 检查是否选择了塔
		if (player_selected_tower_type == entity::invalid_type)
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

		// 建造
		const auto tower_entity = Tower::build(registry, grid_position, player_selected_tower_type);
		if (tower_entity == entt::null)
		{
			std::println("建造塔失败");
			return false;
		}

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
