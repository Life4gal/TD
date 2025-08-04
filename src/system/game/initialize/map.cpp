#include <system/game/initialize/map.hpp>

#include <component/game/map.hpp>
#include <component/game/render.hpp>

#include <entt/entt.hpp>

namespace game::system::initialize
{
	auto map(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		sf::RectangleShape tile_shape{{static_cast<float>(tile_map.tile_width()), static_cast<float>(tile_map.tile_height())}};
		{
			tile_shape.setOutlineColor(sf::Color::Red);
			tile_shape.setOutlineThickness(1.f);
		}

		sf::CircleShape gate_shape{static_cast<float>(tile_map.tile_width()) * .45f};
		{
			gate_shape.setOrigin({gate_shape.getRadius(), gate_shape.getRadius()});
		}

		render::Map render_map
		{
				.tile_shape = std::move(tile_shape),
				.gate_shape = std::move(gate_shape),
		};

		registry.ctx().emplace<render::Map>(std::move(render_map));
	}
}
