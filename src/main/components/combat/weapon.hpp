#pragma once

#include <entt/entity/fwd.hpp>

namespace components::weapon
{
	// 攻击距离
	class Range
	{
	public:
		float range;
	};

	// 开火频率
	class FireRate
	{
	public:
		float fire_rate;
	};

	// 武器冷却(如果处于冷却状态)
	class Cooldown
	{
	public:
		float delay;
	};

	// 开火
	class Trigger
	{
	public:
		using fire_type = auto(*)(entt::registry& registry, entt::entity attacker, entt::entity victim) noexcept -> void;

		fire_type on_fire;
	};

	// 武器目标(如果有目标)
	class Target
	{
	public:
		entt::entity entity;
	};
}
