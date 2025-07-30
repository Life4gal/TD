#pragma once

#include <components/enemy.hpp>

#include <entt/entity/entity.hpp>

namespace components::weapon
{
	enum class SearchStrategy : std::uint8_t
	{
		DISTANCE_FIRST,
		POWER_FIRST,
	};

	class Specification
	{
	public:
		using search_type_type = std::underlying_type_t<enemy::CategoryValue>;

		using fire_type = auto(*)(entt::registry& registry, entt::entity attacker, entt::entity victim) noexcept -> void;

		// 索敌类型
		search_type_type search_type;
		// 索敌策略
		SearchStrategy search_strategy;

		float range;
		float fire_rate;
		float fire_timer;

		fire_type on_fire;
	};

	class Owner
	{
	public:
		entt::entity entity;
	};

	class Target
	{
	public:
		entt::entity entity;
	};
}
