#include <systems/render/player.hpp>

#include <components/player.hpp>
#include <components/map.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems::render
{
	auto player(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		if (auto* render_player = registry.ctx().find<components::render::Player>())
		{
			const auto& [map] = registry.ctx().get<const map_ex::Map>();

			// 绘制游标方框
			{
				const auto mouse_position = sf::Mouse::getPosition(window);
				const auto mouse_grid_position = map.coordinate_world_to_grid(mouse_position);

				if (map.inside(mouse_grid_position.x, mouse_grid_position.y))
				{
					auto& cursor_shape = render_player->cursor_shape;

					const auto world_position = map.coordinate_grid_to_world(mouse_grid_position);
					cursor_shape.setPosition(world_position);

					window.draw(cursor_shape);
				}
			}
		}
	}
}
