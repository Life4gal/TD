#include <system/game/initialize/player.hpp>

#include <component/game/entity.hpp>
#include <component/game/player.hpp>
#include <component/game/map.hpp>
#include <component/game/render.hpp>

#include <entt/entt.hpp>

namespace game::system::initialize
{
	auto player(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		sf::RectangleShape cursor_shape{{static_cast<float>(tile_map.tile_width()), static_cast<float>(tile_map.tile_height())},};

		cursor_shape.setOrigin(cursor_shape.getSize() / 2.f);
		cursor_shape.setFillColor(sf::Color::Transparent);
		cursor_shape.setOutlineColor(sf::Color::Green);
		cursor_shape.setOutlineThickness(2.f);

		render::Player render_player
		{
				.cursor_shape = std::move(cursor_shape),
		};

		registry.ctx().emplace<render::Player>(std::move(render_player));

		registry.ctx().emplace<player::Interaction>(entity::invalid_type, entity::invalid_type);
		registry.ctx().emplace<player::Tower>();
		registry.ctx().emplace<player::Resource>();
		registry.ctx().emplace<player::Statistics>(player::Statistics::size_type{0});
	}
}
