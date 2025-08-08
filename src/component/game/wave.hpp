#pragma once

#include <vector>

#include <component/game/entity.hpp>

#include <SFML/System/Time.hpp>

namespace game::component::wave
{
	// Wave[entity]:
	//  Wave
	//  WaveSpawnIndex
	//  WaveTimer
	//
	// WaveDuration[context]
	// WaveIndex[context]
	// WaveSequence[context]

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
	// 一个波次数据
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

	// 当前波次计数器
	class WaveTimer
	{
	public:
		sf::Time timer;
	};

	// =====================================
	// 所有波次数据
	// =====================================

	// 当前波次持续时间
	// 到达时间后刷新下一个波次
	// 玩家可以提前开始下一波,也就是将该持续时间置为0
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
