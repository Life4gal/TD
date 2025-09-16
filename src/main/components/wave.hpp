#pragma once

#include <config/wave.hpp>

#include <entt/entity/fwd.hpp>

namespace components::wave
{
	using config::wave::index_type;

	// 没有任何波次生成(游戏处于最初准备阶段)
	constexpr index_type wave_not_start = std::numeric_limits<index_type>::max();
	// 所有波次都完全结束
	constexpr index_type wave_all_completed = wave_not_start - 1;

	// ==================
	// 波次实体
	// ==================

	// 波次索引(readonly)
	class WaveIndex
	{
	public:
		index_type index;
	};

	// 波次配置(readonly)
	class Wave
	{
	public:
		using spawns_type = config::wave::Wave::spawns_type;

		// 直接引用config::wave::Wave中的数据
		std::reference_wrapper<const spawns_type> spawns;
	};

	// 波次结束条件(readonly)
	using config::wave::EndCondition;

	// 波次准备时间(如果有,取决于配置)
	class WavePreparationTimer
	{
	public:
		sf::Time remaining;
	};

	// 波次敌人实体(用于检测波次是否满足ENDED=>DEAD)
	class WaveEnemy
	{
	public:
		std::vector<entt::entity> enemy;
	};

	// 波次状态
	enum class WaveState : std::uint8_t
	{
		// 准备阶段,等待用户确认
		PREPARING,
		// 正在生成敌人
		SPAWNING,
		// 生成完毕,但是未满足结束条件
		RUNNING,
		// 已满足结束条件(可以刷新下一波次)
		COMPLETED,
		// 已结束(满足结束条件,但是当前波次还有敌人未死亡)
		ENDED,
		// 波次完全结束(所有敌人全部死亡,波次实体可以销毁)
		DEAD,
	};

	// 生成索引
	class SpawnIndex
	{
	public:
		index_type index;
	};

	// 波次计时器
	class WaveTimer
	{
	public:
		sf::Time elapsed_time;
	};

	// ==================
	// 波次上下文
	// ==================

	// 总的波次数量
	class WaveTotalCount
	{
	public:
		index_type count;
	};

	// 当前生成的波次
	class WaveCurrentIndex
	{
	public:
		index_type index;
	};

	// 当前生成的波次实体
	class WaveCurrentEntity
	{
	public:
		entt::entity entity;
	};
}
