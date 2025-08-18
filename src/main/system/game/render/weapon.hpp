#pragma once

#include <entt/entt.hpp>

namespace sf
{
	class RenderWindow;
}

namespace game::system::render
{
	auto weapon(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
}
