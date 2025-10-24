#include <render/navigation.hpp>

#include <components/map.hpp>
#include <components/navigation.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace render
{
	auto navigation(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		// 绘制缓存路径
		{
			const auto& [cache_paths] = registry.ctx().get<const navigation::Path>();

			sf::VertexArray paths{sf::PrimitiveType::LineStrip};

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
		{
			const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();

			sf::VertexArray directions{sf::PrimitiveType::Lines};

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
	}
}
