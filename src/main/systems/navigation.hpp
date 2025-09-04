#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems
{
	class Navigation
	{
	public:
		static auto update(entt::registry& registry, sf::Time delta) noexcept -> void;
	};
}
