#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems
{
	// ReSharper disable once CppInconsistentNaming
	class HUD
	{
	public:
		static auto update(entt::registry& registry) noexcept -> void;

		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
	};
}
