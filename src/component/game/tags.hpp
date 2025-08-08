#pragma once

#include <entt/core/type_traits.hpp>
#include <entt/core/hashed_string.hpp>

namespace game::component::tags
{
	using entt::literals::operator ""_hs;

	// ====================================
	// WAVE

	namespace wave
	{
		using identifier = entt::tag<"Wave"_hs>;

		// =======
		// 状态

		namespace status
		{
			// 正在生成
			// using spawning = entt::tag<"WaveSpawning"_hs>;
			// 生成完毕
			using end = entt::tag<"WaveEnd"_hs>;
		}
	}

	// ====================================
	// TOWER

	namespace tower
	{
		using identifier = entt::tag<"Tower"_hs>;

		// =======
		// 状态

		namespace status
		{
			// 是否正在建造中
			// using building = entt::tag<"TowerStatusBuilding"_hs>;
			// 是否正在拆除中
			// using destructing = entt::tag<"TowerStatusDestructing"_hs>;
		}
	}

	// ====================================
	// WEAPON
	namespace weapon
	{
		using identifier = entt::tag<"Weapon"_hs>;

		// =======
		// 状态

		namespace status
		{
			// 是否可用
			using disabled = entt::tag<"WeaponStatusDisabled"_hs>;
			// 是否正在索敌
			using searching = entt::tag<"WeaponStatusSearching"_hs>;
			// 是否正在攻击
			using attacking = entt::tag<"WeaponStatusAttacking"_hs>;
			// 是否攻击冷却中
			// weapon::Cooldown
			// using cooldown = entt::tag<"WeaponStatusCooldown"_hs>;
		}

		// =======
		// 临时状态(Buff/Debuff)

		namespace condition
		{
			//
		}

		// =======
		// 索敌类型

		namespace targeting
		{
			// 地面
			using ground = entt::tag<"WeaponTargetingGround"_hs>;
			// 空中
			using air = entt::tag<"WeaponTargetingAir"_hs>;
			// 对地+对空
			// 同时包含两个标签即可
			// using dual = entt::tag<"WeaponTargetingDual"_hs>;
		}

		// =======
		// 索敌策略

		namespace strategy
		{
			// 距离优先
			using distance = entt::tag<"WeaponStrategyDistance"_hs>;
			// 强度优先
			using power = entt::tag<"WeaponStrategyPower"_hs>;
		}
	}

	// ====================================
	// ENEMY

	namespace enemy
	{
		using identifier = entt::tag<"Enemy"_hs>;

		// =======
		// 状态

		namespace status
		{
			// 是否存活
			using alive = entt::tag<"EnemyStatusAlive"_hs>;
			// 是否被(塔)击杀
			using dead = entt::tag<"EnemyStatusDead"_hs>;
			// 是否到达终点
			using reached = entt::tag<"EnemyStatusReached"_hs>;
			// 是否可见
			using visible = entt::tag<"EnemyStatusVisible"_hs>;
		}

		// =======
		// 临时状态(Buff/Debuff)

		namespace condition
		{
			// 能否被攻击(造成伤害)
			using immortal = entt::tag<"EnemyConditionImmortal"_hs>;
		}

		// =======
		// 移动方式

		namespace archetype
		{
			// 地面移动
			using ground = entt::tag<"EnemyArchetypeGround"_hs>;
			// 空中移动
			using aerial = entt::tag<"EnemyArchetypeAerial"_hs>;
		}
	}
}
