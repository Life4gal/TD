#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

namespace render
{
	auto renderable(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
}
