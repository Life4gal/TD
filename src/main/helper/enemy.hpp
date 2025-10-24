#pragma once

#include <entt/fwd.hpp>

namespace helper
{
	class Enemy
	{
	public:
		// 杀死敌人(死因: 到达终点)
		static auto reach(entt::registry& registry, entt::entity enemy) noexcept -> void;

		// 杀死敌人(死因: 被杀死)
		static auto kill(entt::registry& registry, entt::entity attacker, entt::entity victim) noexcept -> void;

		// 伤害敌人(如果伤害足够则杀死敌人)
		static auto hurt(entt::registry& registry, entt::entity attacker, entt::entity victim, float damage) noexcept -> void;
	};
}
