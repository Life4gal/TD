#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace systems
{
	class Game
	{
	public:
		static auto update(entt::registry& registry, sf::Time delta) noexcept -> void;

		static auto update_simulation(entt::registry& registry, sf::Time delta) noexcept -> void;
	};
}
