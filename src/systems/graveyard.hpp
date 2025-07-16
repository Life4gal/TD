#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems
{
	class Graveyard
	{
	public:
		static auto initialize(entt::registry& registry) noexcept -> void;

		static auto update(entt::registry& registry, sf::Time delta) noexcept -> void;

		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
	};
}
