#pragma once

#include <components/entity.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace helper
{
	class Enemy
	{
	public:
		static auto spawn(entt::registry& registry, sf::Vector2u point, components::entity::Type type) noexcept -> entt::entity;

		static auto spawn(entt::registry& registry, std::uint32_t start_gate_id, components::entity::Type type) noexcept -> entt::entity;
	};
}
