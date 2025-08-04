#pragma once

#include <component/game/entity.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace game::system::helper
{
	class Enemy
	{
	public:
		// =============================
		// 生成敌人

		static auto spawn(entt::registry& registry, sf::Vector2u point, component::entity::Type enemy_type) noexcept -> entt::entity;

		static auto spawn(entt::registry& registry, std::uint32_t start_gate_id, component::entity::Type enemy_type) noexcept -> entt::entity;
	};
}
