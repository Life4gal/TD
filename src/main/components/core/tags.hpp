#pragma once

#include <entt/core/type_traits.hpp>
#include <entt/core/hashed_string.hpp>

namespace components::tags
{
	using entt::literals::operator""_hs;

	// 波次标识符
	using wave = entt::tag<"Wave"_hs>;

	// 塔标识符
	using tower = entt::tag<"Tower"_hs>;

	// 敌人标识符
	using enemy = entt::tag<"Enemy"_hs>;

	// 实体不可见
	using invisible = entt::tag<"Invisible"_hs>;
	// 实体已死亡
	using dead = entt::tag<"Dead"_hs>;

	// 敌人死因(cause of death)
	// 敌人到达终点
	using cod_reached = entt::tag<"CauseOfDeathReached"_hs>;
	// 敌人被杀死
	using cod_killed = entt::tag<"CauseOfDeathKilled"_hs>;

	// 索敌类型
	// 对地面
	using targeting_ground = entt::tag<"TargetingGround"_hs>;
	// 对空中
	using targeting_air = entt::tag<"TargetingAir"_hs>;
	// 对空中+对地面
	// 同时包含两个标签即可
	// using targeting_dual = entt::tag<"TargetingDual"_hs>;

	// 索敌策略
	// 地面优先
	using strategy_ground_first = entt::tag<"StrategyGroundFirst"_hs>;
	// 空中优先
	using strategy_air_first = entt::tag<"StrategyAirFirst"_hs>;
	// 距离优先
	using strategy_distance_first = entt::tag<"StrategyDistanceFirst"_hs>;
	// 强度优先
	using strategy_power_first = entt::tag<"StrategyPowerFirst"_hs>;

	// 移动方式
	// 地面移动
	using archetype_ground = entt::tag<"ArchetypeGround"_hs>;
	// 空中移动
	using archetype_aerial = entt::tag<"ArchetypeAerial"_hs>;
}
