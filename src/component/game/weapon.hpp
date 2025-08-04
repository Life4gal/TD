#pragma once

#include <entt/entity/entity.hpp>

namespace game::component::weapon
{
	// 拥有武器的塔
	class Owner
	{
	public:
		entt::entity entity;
	};

	class Specification
	{
	public:
		using fire_type = auto(*)(entt::registry& registry, entt::entity attacker, entt::entity victim) noexcept -> void;

		float range;
		float fire_rate;

		fire_type on_fire;
	};

	class Cooldown
	{
	public:
		float delay;
	};

	class Target
	{
	public:
		entt::entity entity;
	};
}
