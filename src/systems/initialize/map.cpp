#include <systems/initialize/map.hpp>

#include <components/map.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>

namespace systems::initialize
{
	auto map(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::Map>();

		render::Map render_map
		{
				.tile_shape = sf::RectangleShape{{static_cast<float>(tile_map.tile_width()), static_cast<float>(tile_map.tile_height())},},
				.gate_shape = sf::CircleShape{static_cast<float>(std::ranges::min(tile_map.tile_width(), tile_map.tile_height())) * .45f},
		};

		// tile
		{
			auto& tile_shape = render_map.tile_shape;

			tile_shape.setOutlineColor(sf::Color::Red);
			tile_shape.setOutlineThickness(1.f);
		}
		// gate
		{
			auto& gate_shape = render_map.gate_shape;

			gate_shape.setOrigin({gate_shape.getRadius(), gate_shape.getRadius()});
		}

		registry.ctx().emplace<render::Map>(std::move(render_map));
	}
}
