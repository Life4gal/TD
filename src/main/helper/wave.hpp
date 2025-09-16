#pragma once

#include <components/wave.hpp>

#include <entt/fwd.hpp>

namespace helper
{
	class Wave
	{
	public:
		// 载入波次数据
		static auto load(entt::registry& registry) noexcept -> void;

		// =========================
		// 状态切换

		// todo: 使用状态机确保状态切换正确?
		class StateMachine
		{
			class StateChanged
			{
			public:
				using state_type = components::wave::WaveState;

				entt::entity wave;
				state_type old_state;
				state_type new_state;
			};

			// 所有波次完成
			class AllCompleted {};

			// 波次状态切换
			static auto on(entt::registry& registry, const StateChanged& state) noexcept -> void;

			// 所有波次完成
			static auto on(entt::registry& registry, const AllCompleted& event) noexcept -> void;

		public:
			// 准备阶段结束 (WaveState::PREPARING ==> WaveState::SPAWNING)
			class PreparationEnded
			{
			public:
				entt::entity wave;
			};

			// 波次所有敌人生成完成 (WaveState::SPAWNING ==> WaveState::RUNNING)
			class SpawnsCompleted
			{
			public:
				entt::entity wave;
			};

			// 满足波次结束条件 (WaveState::RUNNING ==> WaveState::COMPLETED)
			class ConditionMet
			{
			public:
				entt::entity wave;
			};

			// 波次完成 (WaveState::COMPLETED ==> WaveState::ENDED)
			class Completed
			{
			public:
				entt::entity wave;
			};

			// 波次结束 (WaveState::ENDED ==> WaveState::DEAD)
			class Finished
			{
			public:
				entt::entity wave;
			};

			// 跳过准备时间请求
			class SkipPreparationRequested {};

			// 下一波次提前开始请求
			class EarlyStartRequested {};

			// WaveState::PREPARING ==> WaveState::SPAWNING
			static auto on(entt::registry& registry, const PreparationEnded& event) noexcept -> void;

			// WaveState::SPAWNING ==> WaveState::RUNNING
			static auto on(entt::registry& registry, const SpawnsCompleted& event) noexcept -> void;

			// WaveState::RUNNING ==> WaveState::COMPLETED
			static auto on(entt::registry& registry, const ConditionMet& event) noexcept -> void;

			// WaveState::COMPLETED ==> WaveState::ENDED
			static auto on(entt::registry& registry, const Completed& event) noexcept -> void;

			// WaveState::ENDED ==> WaveState::DEAD
			static auto on(entt::registry& registry, const Finished& event) noexcept -> void;

			// 跳过准备时间请求
			static auto on(entt::registry& registry, const SkipPreparationRequested& event) noexcept -> void;

			// 下一波次提前开始请求
			static auto on(entt::registry& registry, const EarlyStartRequested& event) noexcept -> void;
		};

		// =========================

		// 检查制定波次是否满足结束条件
		[[nodiscard]] static auto check_end_condition(entt::registry& registry, entt::entity wave) noexcept -> bool;

		// 是否还有下一波次
		[[nodiscard]] static auto has_next_wave(entt::registry& registry) noexcept -> bool;

		// 生成指定波次(仅此而已)
		static auto spawn_wave(entt::registry& registry, components::wave::WaveIndex wave_index) noexcept -> entt::entity;

		// 从指定波次开始
		static auto start_from_wave(entt::registry& registry, components::wave::WaveIndex wave_index) noexcept -> void;

		// 立刻开始当前波次(基于设定的波次索引)(不考虑是否满足当前波次结束条件)
		static auto start_new_wave(entt::registry& registry) noexcept -> void;
	};
}
