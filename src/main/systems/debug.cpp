#include <systems/debug.hpp>

#include <components/map.hpp>
#include <components/navigation.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/weapon.hpp>

#include <components/debug.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Debug::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		// 绘制缓存路径
		if (auto* render = registry.ctx().find<DebugRenderPath>())
		{
			const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

			const auto& [cache_paths] = registry.ctx().get<const navigation::Path>();

			auto& paths = render->paths;

			std::ranges::for_each(
				cache_paths,
				[&](const auto& path) noexcept -> void
				{
					std::ranges::for_each(
						path,
						[&](const auto point) noexcept -> void
						{
							const auto world_position = tile_map.coordinate_grid_to_world(point);

							paths.append({.position = world_position, .color = sf::Color::Green, .texCoords = {}});
						}
					);

					window.draw(paths);
					paths.clear();
				}
			);
		}

		// 绘制网格流向
		if (auto* render = registry.ctx().find<DebugRenderFlow>())
		{
			const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

			const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();

			auto& directions = render->directions;

			for (std::uint32_t y = 0; y < tile_map.vertical_tile_count(); ++y)
			{
				for (std::uint32_t x = 0; x < tile_map.horizontal_tile_count(); ++x)
				{
					constexpr auto arrow_length = 15.f;
					constexpr auto arrow_head_length = arrow_length * .35f;

					const auto direction = flow_field.direction_of({x, y});
					const auto direction_value = sf::Vector2f{map::value_of(direction)};

					const auto start_position = tile_map.coordinate_grid_to_world(sf::Vector2u{x, y});
					const auto end_position = start_position + direction_value * arrow_length;
					const auto arrow_offset = direction_value * arrow_head_length;

					directions.append({.position = start_position, .color = sf::Color::Red, .texCoords = {}});
					directions.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});

					const auto perpendicular = direction_value.perpendicular();
					const auto arrow_position_1 = end_position - arrow_offset + perpendicular * arrow_head_length * .5f;
					const auto arrow_position_2 = end_position - arrow_offset - perpendicular * arrow_head_length * .5f;

					directions.append({.position = arrow_position_1, .color = sf::Color::Red, .texCoords = {}});
					directions.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});
					directions.append({.position = arrow_position_2, .color = sf::Color::Red, .texCoords = {}});
					directions.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});
				}
			}

			window.draw(directions);
			directions.clear();
		}

		// 绘制塔的目标线
		if (auto* render = registry.ctx().find<DebugRenderTarget>())
		{
			auto& lines = render->lines;

			for (const auto tower_view = registry.view<const weapon::Target, tags::tower>();
			     const auto [entity, target]: tower_view.each())
			{
				// 目标可能已经被别的塔击杀
				// if (registry.all_of<tags::dead>(target.entity))
				if (not registry.valid(target.entity))
				{
					continue;
				}

				const auto [tower_position] = registry.get<const entity::Position>(entity);
				const auto [enemy_position] = registry.get<const entity::Position>(target.entity);

				lines.append({.position = tower_position, .color = sf::Color::Red, .texCoords = {}});
				lines.append({.position = enemy_position, .color = sf::Color::Red, .texCoords = {}});
			}

			window.draw(lines);
			lines.clear();
		}
	}
}
