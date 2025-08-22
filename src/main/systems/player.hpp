#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems
{
	class Player
	{
	public:
		// ===================================================

		static auto initialize(entt::registry& registry) noexcept -> void;

		static auto update(entt::registry& registry) noexcept -> void;

		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;

		// ===================================================

		// 建造/销毁塔

		static auto try_build_tower(entt::registry& registry, sf::Vector2f position) noexcept -> bool;

		static auto try_destroy_tower(entt::registry& registry, sf::Vector2f position) noexcept -> bool;
	};
}
