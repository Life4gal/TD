#include <render/player.hpp>

#include <components/player.hpp>
#include <components/map.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace render
{
	auto player(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		// 绘制游标方框
		{
			const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

			auto& [cursor] = registry.ctx().get<player::Cursor>();

			const auto mouse_position = sf::Mouse::getPosition(window);
			const auto mouse_grid_position = tile_map.coordinate_world_to_grid(sf::Vector2f{mouse_position});

			if (tile_map.inside(mouse_grid_position.x, mouse_grid_position.y))
			{
				const auto world_position = tile_map.coordinate_grid_to_world(mouse_grid_position);
				cursor.setPosition(world_position);

				window.draw(cursor);
			}
		}
	}
}
