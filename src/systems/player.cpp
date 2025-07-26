#include <systems/player.hpp>

#include <print>

#include <components/tags.hpp>
#include <components/tower.hpp>
#include <components/render.hpp>

#include <components/map.hpp>
#include <components/navigation.hpp>
#include <components/player.hpp>

#include <systems/tower.hpp>
#include <systems/resource.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Player::initialize(entt::registry& registry) noexcept -> void
	{
		components::PlayerData player_data
		{
				.selected_tower_type = {components::EntityType::invalid_type},
				.resources = {},
				.towers = {},
		};

		// render
		{
			const auto& map_data = registry.ctx().get<components::MapData>();
			const auto& map = map_data.map;

			sf::RectangleShape cursor_shape{{static_cast<float>(map.tile_width()), static_cast<float>(map.tile_height())},};

			cursor_shape.setOrigin(cursor_shape.getSize() / 2.f);
			cursor_shape.setFillColor(sf::Color::Transparent);
			cursor_shape.setOutlineColor(sf::Color::Green);
			cursor_shape.setOutlineThickness(2.f);

			components::RenderPlayerData render_player_data
			{
					.cursor_shape = std::move(cursor_shape),
			};

			registry.ctx().emplace<components::RenderPlayerData>(std::move(render_player_data));
		}

		registry.ctx().emplace<components::PlayerData>(std::move(player_data));
	}

	auto Player::update(entt::registry& registry) noexcept -> void
	{
		auto& player_data = registry.ctx().get<components::PlayerData>();
		auto& resources = player_data.resources;
		auto& health = resources[components::ResourceType::HEALTH];

		const auto enemy_reached_view = registry.view<components::tags::enemy, components::tags::enemy_reached>();
		const auto reached_count = enemy_reached_view.size_hint();

		// todo: 每个敌人扣多少生命值?
		if (const auto cost_health = reached_count * 1;
			cost_health >= health)
		{
			// todo: 游戏结束
		}
		else
		{
			health -= cost_health;
		}
	}

	auto Player::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		if (auto* render_player_data = registry.ctx().find<components::RenderPlayerData>())
		{
			const auto& map_data = registry.ctx().get<components::MapData>();
			auto& map = map_data.map;

			// 绘制游标方框
			{
				const auto mouse_position = sf::Mouse::getPosition(window);
				const auto mouse_grid_position = map.coordinate_world_to_grid(mouse_position);

				if (map.inside(mouse_grid_position.x, mouse_grid_position.y))
				{
					auto& cursor_shape = render_player_data->cursor_shape;

					const auto world_position = map.coordinate_grid_to_world(mouse_grid_position);
					cursor_shape.setPosition(world_position);

					window.draw(cursor_shape);
				}
			}
		}
	}

	auto Player::try_build_tower(entt::registry& registry, const sf::Vector2f world_position) noexcept -> bool
	{
		auto& map_data = registry.ctx().get<components::MapData>();
		auto& map = map_data.map;

		auto& player_data = registry.ctx().get<components::PlayerData>();

		auto& navigation_data = registry.ctx().get<components::NavigationData>();
		auto& flow_field = navigation_data.flow_field;

		const auto grid_position = map.coordinate_world_to_grid(world_position);

		// 检查是否选择了塔
		if (player_data.selected_tower_type.type == components::EntityType::invalid_type)
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
		if (not Resource::require(registry, player_data.selected_tower_type))
		{
			std::println("资源不足");
			return false;
		}

		// 检查路径是否会被堵死
		std::vector<std::reference_wrapper<map::path_type>> changed_cache_path{};
		{
			map.set(grid_position.x, grid_position.y, map::TileType::TOWER);

			// 只要有一条路线被堵死就不允许建造
			for (auto& cache_path: navigation_data.cache_paths)
			{
				// 建造位置不在路径上不会影响路径
				if (not std::ranges::contains(cache_path, grid_position))
				{
					continue;
				}

				if (not map::PathFinder::is_reachable(
					map,
					cache_path.front(),
					map_data.end_gates
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
		const auto entity = Tower::build(registry, grid_position, player_data.selected_tower_type);
		if (entity == entt::null)
		{
			std::println("建造失败");
			return false;
		}

		// 消耗资源
		Resource::consume_unchecked(registry, player_data.selected_tower_type);

		// 记录建造的塔
		player_data.towers.emplace(grid_position, entity);

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
		auto& map_data = registry.ctx().get<components::MapData>();
		auto& map = map_data.map;

		auto& player_data = registry.ctx().get<components::PlayerData>();

		auto& navigation_data = registry.ctx().get<components::NavigationData>();
		auto& flow_field = navigation_data.flow_field;

		const auto grid_position = map.coordinate_world_to_grid(world_position);

		// 检查位置是否合法
		if (not map.inside(grid_position.x, grid_position.y) or map.at(grid_position.x, grid_position.y) != map::TileType::TOWER)
		{
			std::println("位置({}:{})不在地图内或未建造塔", grid_position.x, grid_position.y);
			return false;
		}

		const auto it = player_data.towers.find(grid_position);
		assert(it != player_data.towers.end());

		const auto& [weapons] = registry.get<components::Equipment>(it->second);

		// 返还资源
		{
			// 武器
			if (not weapons.empty())
			{
				std::vector<components::EntityType> types{};
				types.reserve(weapons.size());

				std::ranges::transform(
					weapons,
					std::back_inserter(types),
					[&](const entt::entity entity) noexcept -> components::EntityType
					{
						return registry.get<components::EntityType>(entity);
					}
				);

				Resource::acquire(registry, types);
			}

			// 塔
			const auto tower_type = registry.get<components::EntityType>(it->second);
			Resource::acquire(registry, tower_type);
		}

		// 销毁塔
		{
			if (not weapons.empty())
			{
				registry.destroy(weapons.begin(), weapons.end());
			}
			registry.destroy(it->second);
			player_data.towers.erase(it);
		}

		// 设置地块
		{
			map.set(grid_position.x, grid_position.y, map::TileType::BUILDABLE_FLOOR);
			map_data.tower_counter -= 1;
		}

		// 更新流场
		flow_field.update(grid_position);

		return true;
	}
}
