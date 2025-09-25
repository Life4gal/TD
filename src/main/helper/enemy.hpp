#pragma once

#include <components/entity.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace helper
{
	class Enemy
	{
	public:
		// 生成敌人(任意位置)
		static auto spawn(entt::registry& registry, sf::Vector2u point, components::entity::Type type) noexcept -> entt::entity;

		// 生成敌人(指定出生点)
		static auto spawn(entt::registry& registry, std::uint32_t start_gate_id, components::entity::Type type) noexcept -> entt::entity;

		// 杀死敌人(死因: 到达终点)
		static auto reach(entt::registry& registry, entt::entity enemy) noexcept -> void;

		// 杀死敌人(死因: 被杀死)
		static auto kill(entt::registry& registry, entt::entity attacker, entt::entity victim) noexcept -> void;

		// 伤害敌人(如果伤害足够则杀死敌人)
		static auto hurt(entt::registry& registry, entt::entity attacker, entt::entity victim, float damage) noexcept -> void;
	};
}
