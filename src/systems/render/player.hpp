#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems::render
{
	auto player(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
}
