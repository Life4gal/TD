#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

namespace game::system::render
{
	auto map(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
}
