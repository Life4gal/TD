#pragma once

#include <components/entity.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems
{
	class Tower
	{
	public:
		static auto initialize(entt::registry& registry) noexcept -> void;

		static auto update(entt::registry& registry, sf::Time delta) noexcept -> void;

		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;

		// =============================
		// 建造塔

		static auto build(entt::registry& registry, sf::Vector2u grid_position, components::EntityType tower_type) noexcept -> entt::entity;
	};
}
