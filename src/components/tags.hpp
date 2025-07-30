#pragma once

#include <entt/core/type_traits.hpp>
#include <entt/core/hashed_string.hpp>

namespace components::tags
{
	using entt::literals::operator ""_hs;

	// ====================================
	// RESOURCE

	using resource = entt::tag<"Resource"_hs>;

	// ====================================
	// TOWER

	using tower = entt::tag<"Tower"_hs>;

	// 正在建造中
	// using tower_building = entt::tag<"TowerBuilding"_hs>;
	// 正在拆除中
	// using tower_destructing = entt::tag<"TowerDestructing"_hs>;

	// ====================================
	// TOWER

	using weapon = entt::tag<"Weapon"_hs>;

	// 是否正在索敌
	using weapon_searching = entt::tag<"WeaponSearching"_hs>;
	// 是否正在攻击
	using weapon_attacking = entt::tag<"WeaponAttacking"_hs>;
	// 是否攻击冷却中
	using weapon_cooldown = entt::tag<"WeaponCooldown"_hs>;

	// ====================================
	// ENEMY

	using enemy = entt::tag<"Enemy"_hs>;

	// 是否存活
	using enemy_alive = entt::tag<"EnemyAlive"_hs>;
	// 是否被(塔)击杀
	using enemy_killed = entt::tag<"EnemyKilled"_hs>;
	// 是否到达终点
	using enemy_reached = entt::tag<"EnemyReached"_hs>;

	// 能否被作为攻击目标
	using enemy_aimable = entt::tag<"EnemyAimable"_hs>;
	// 能否被攻击(造成伤害)
	using enemy_immortal = entt::tag<"EnemyImmortal"_hs>;
}
