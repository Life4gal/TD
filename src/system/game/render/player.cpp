#include <system/game/render/player.hpp>

#include <component/game/map.hpp>
#include <component/game/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace game::system::render
{
	auto player(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace component;

		if (auto* render_player = registry.ctx().find<component::render::Player>())
		{
			const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

			// 绘制游标方框
			{
				const auto mouse_position = sf::Mouse::getPosition(window);
				const auto mouse_grid_position = tile_map.coordinate_world_to_grid(mouse_position);

				if (tile_map.inside(mouse_grid_position.x, mouse_grid_position.y))
				{
					auto& cursor_shape = render_player->cursor_shape;

					const auto world_position = tile_map.coordinate_grid_to_world(mouse_grid_position);
					cursor_shape.setPosition(world_position);

					window.draw(cursor_shape);
				}
			}
		}
	}
}
