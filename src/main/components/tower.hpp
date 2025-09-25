#pragma once

#include <entt/entity/fwd.hpp>

namespace components::tower
{
	// 武器配置
	class Weapon
	{
	public:
		float range;
		float fire_rate;
	};

	// 开火
	class Trigger
	{
	public:
		using fire_type = auto (*)(entt::registry& registry, entt::entity attacker, entt::entity victim) noexcept -> void;

		fire_type on_fire;
	};

	// 武器冷却(如果处于冷却状态)
	class Cooldown
	{
	public:
		float delay;
	};

	// 武器目标(如果有目标)
	class Target
	{
	public:
		entt::entity entity;
	};
}
