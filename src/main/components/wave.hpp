#pragma once

#include <components/entity.hpp>

#include <SFML/System/Time.hpp>

namespace components::wave
{
	// Wave[entity]:
	//  Wave (当前波次所有生成信息)
	//  WaveSpawnIndex (当前波次正在生成的波次索引)
	//  WaveTimer (当前波次历时)
	//  WaveIndex (当前波次在所有波次中的索引)
	//
	// WaveDuration[context] (当前波次剩余持续时间,或者说下一个波次刷新时间)
	// WaveIndex[context] (当前刷新的波次索引)
	// WaveSequence[context] (所有波次信息)

	// 单个敌人生成
	class Spawn
	{
	public:
		// 敌人类型
		entity::Type type;
		// 出生点
		std::uint32_t gate_id;
		// 该波次开始后多久进行该次生成
		sf::Time delay;
	};

	// =====================================
	// 一个波次的数据
	// =====================================

	// 此波次生成敌人的信息
	class Wave
	{
	public:
		// 假定已经按照Spawn::delay排序
		std::vector<Spawn> spawns;
	};

	// 生成索引
	enum class WaveSpawnIndex : std::uint32_t {};

	// 当前波次计时器
	class WaveTimer
	{
	public:
		sf::Time timer;
	};

	// =====================================
	// 所有波次的数据
	// =====================================

	// 当前波次持续时间
	// 到达时间后刷新下一个波次
	// 玩家可以提前开始下一波
	class WaveDuration
	{
	public:
		sf::Time duration;
	};

	// 波次索引
	enum class WaveIndex : std::uint32_t {};

	// 波次信息
	class WaveSequence
	{
	public:
		// 所有波次
		std::vector<Wave> waves;
		// 所有波次持续时间
		std::vector<WaveDuration> durations;
	};
}
