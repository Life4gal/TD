#pragma once

#include <components/entity.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace systems::helper
{
	class Enemy
	{
	public:
		// =============================
		// 生成敌人

		static auto spawn(entt::registry& registry, sf::Vector2u point, components::EntityType enemy_type) noexcept -> entt::entity;

		static auto spawn(entt::registry& registry, std::uint32_t start_gate_id, components::EntityType enemy_type) noexcept -> entt::entity;
	};
}
