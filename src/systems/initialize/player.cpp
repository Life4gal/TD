#include <systems/initialize/player.hpp>

#include <components/entity.hpp>
#include <components/player.hpp>
#include <components/map.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>

namespace systems::initialize
{
	auto player(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		// render
		{
			const auto& [map] = registry.ctx().get<const map_ex::Map>();

			sf::RectangleShape cursor_shape{{static_cast<float>(map.tile_width()), static_cast<float>(map.tile_height())},};

			cursor_shape.setOrigin(cursor_shape.getSize() / 2.f);
			cursor_shape.setFillColor(sf::Color::Transparent);
			cursor_shape.setOutlineColor(sf::Color::Green);
			cursor_shape.setOutlineThickness(2.f);

			render::Player render_player
			{
					.cursor_shape = std::move(cursor_shape),
			};

			registry.ctx().emplace<render::Player>(std::move(render_player));
		}

		registry.ctx().emplace<player::Interaction>(EntityType{EntityType::invalid_type}, EntityType{EntityType::invalid_type});
		registry.ctx().emplace<player::Resource>();
		registry.ctx().emplace<player::Tower>();
	}
}
