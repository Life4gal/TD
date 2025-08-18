#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

namespace game::system::render
{
	auto hud(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
}
