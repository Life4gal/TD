#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems
{
	class Tower
	{
	public:
		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
	};
}
