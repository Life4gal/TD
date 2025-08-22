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
	class Enemy
	{
	public:
		// ===================================================

		static auto initialize(entt::registry& registry) noexcept -> void;

		static auto update(entt::registry& registry, sf::Time delta) noexcept -> void;

		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;

		// ===================================================

		static auto spawn(entt::registry& registry, sf::Vector2u point, components::entity::Type type) noexcept -> entt::entity;

		static auto spawn(entt::registry& registry, std::uint32_t start_gate_id, components::entity::Type type) noexcept -> entt::entity;
	};
}
