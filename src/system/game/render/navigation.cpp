#include <system/game/render/navigation.hpp>

#include <component/game/navigation.hpp>
#include <component/game/map.hpp>
#include <component/game/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace game::system::render
{
	auto navigation(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace component;

		if (auto* render_navigation = registry.ctx().find<component::render::Navigation>())
		{
			const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

			const auto& [cache_paths] = registry.ctx().get<const navigation::Path>();

			auto& path_vertices = render_navigation->path_vertices;

			std::ranges::for_each(
				cache_paths,
				[&](const auto& path) noexcept -> void
				{
					std::ranges::for_each(
						path,
						[&](const auto point) noexcept -> void
						{
							const auto world_position = tile_map.coordinate_grid_to_world(point);

							path_vertices.append({.position = world_position, .color = sf::Color::Green, .texCoords = {}});
						}
					);

					window.draw(path_vertices);
					path_vertices.clear();
				}
			);

			// 绘制流场方向
			const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();
			sf::VertexArray lines{sf::PrimitiveType::Lines};
			const auto arrow_length = static_cast<float>(std::ranges::min(tile_map.tile_width(), tile_map.tile_height())) * .5f;
			const auto arrow_head_length = arrow_length * .35f;
			for (map::TileMap::size_type y = 0; y < tile_map.vertical_tile_count(); ++y)
			{
				for (map::TileMap::size_type x = 0; x < tile_map.horizontal_tile_count(); ++x)
				{
					const auto direction = flow_field.direction_of({x, y});
					const auto direction_value = sf::Vector2f{map::value_of(direction)};

					const auto start_position = tile_map.coordinate_grid_to_world(sf::Vector2u{x, y});
					const auto end_position = start_position + direction_value * arrow_length;
					const auto arrow_offset = direction_value * arrow_head_length;

					lines.append({.position = start_position, .color = sf::Color::Red, .texCoords = {}});
					lines.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});

					const auto perpendicular = direction_value.perpendicular();
					const auto arrow_position_1 = end_position - arrow_offset + perpendicular * arrow_head_length * .5f;
					const auto arrow_position_2 = end_position - arrow_offset - perpendicular * arrow_head_length * .5f;

					lines.append({.position = arrow_position_1, .color = sf::Color::Red, .texCoords = {}});
					lines.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});
					lines.append({.position = arrow_position_2, .color = sf::Color::Red, .texCoords = {}});
					lines.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});
				}
			}

			window.draw(lines);
		}
	}
}
