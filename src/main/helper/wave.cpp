#include <helper/wave.hpp>

#include <algorithm>
#include <ranges>
#include <print>

#include <components/core/tags.hpp>

#include <utility/functional.hpp>
#include <utility/time.hpp>

#include <meta/enumeration.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Wave::StateMachine::on(entt::registry& registry, const StateChanged& state) noexcept -> void
	{
		using namespace gal::prometheus;
		using namespace components;

		const auto [index] = registry.get<const wave::WaveIndex>(state.wave);

		std::println(
			"[{:%Y-%m-%d %H:%M:%S}] 波次{}状态切换: {} -> {}(EID:{})",
			utility::zoned_now(),
			index,
			meta::name_of(state.old_state),
			meta::name_of(state.new_state),
			std::to_underlying(state.wave)
		);
	}

	auto Wave::StateMachine::on(entt::registry& registry, const AllCompleted& event) noexcept -> void
	{
		std::println(
			"[{:%Y-%m-%d %H:%M:%S}] ALL WAVES CLEARED!",
			utility::zoned_now()
		);
	}

	auto Wave::StateMachine::on(entt::registry& registry, const PreparationEnded& event) noexcept -> void
	{
		using namespace components;

		assert(registry.valid(event.wave));

		auto& wave_state = registry.get<wave::WaveState>(event.wave);

		assert(wave_state == wave::WaveState::PREPARING);
		constexpr auto new_state = wave::WaveState::SPAWNING;
		const auto old_state = std::exchange(wave_state, new_state);

		on(registry, StateChanged{.wave = event.wave, .old_state = old_state, .new_state = new_state});
	}

	auto Wave::StateMachine::on(entt::registry& registry, const SpawnsCompleted& event) noexcept -> void
	{
		using namespace components;

		assert(registry.valid(event.wave));

		auto& wave_state = registry.get<wave::WaveState>(event.wave);

		assert(wave_state == wave::WaveState::SPAWNING);
		constexpr auto new_state = wave::WaveState::RUNNING;
		const auto old_state = std::exchange(wave_state, new_state);

		on(registry, StateChanged{.wave = event.wave, .old_state = old_state, .new_state = new_state});
	}

	auto Wave::StateMachine::on(entt::registry& registry, const ConditionMet& event) noexcept -> void
	{
		using namespace components;

		assert(registry.valid(event.wave));

		auto& wave_state = registry.get<wave::WaveState>(event.wave);

		assert(wave_state == wave::WaveState::RUNNING);
		constexpr auto new_state = wave::WaveState::COMPLETED;
		const auto old_state = std::exchange(wave_state, new_state);

		on(registry, StateChanged{.wave = event.wave, .old_state = old_state, .new_state = new_state});
	}

	auto Wave::StateMachine::on(entt::registry& registry, const Completed& event) noexcept -> void
	{
		using namespace components;

		assert(registry.valid(event.wave));

		{
			auto& wave_state = registry.get<wave::WaveState>(event.wave);

			assert(wave_state == wave::WaveState::COMPLETED);
			constexpr auto new_state = wave::WaveState::ENDED;
			const auto old_state = std::exchange(wave_state, new_state);

			on(registry, StateChanged{.wave = event.wave, .old_state = old_state, .new_state = new_state});
		}

		// 如果完成的波次是当前波次,则检查是否需要开始下一波次
		if (const auto& [wave_current_entity] = registry.ctx().get<const wave::WaveCurrentEntity>();
			wave_current_entity == event.wave)
		{
			if (has_next_wave(registry))
			{
				// 总是假定满足结束条件才会完成波次
				assert(check_end_condition(registry, event.wave));

				start_new_wave(registry);
			}
			else
			{
				registry.ctx().insert_or_assign(wave::WaveCurrentIndex{.index = wave::wave_all_completed});

				on(registry, AllCompleted{});
			}
		}
	}

	auto Wave::StateMachine::on(entt::registry& registry, const Finished& event) noexcept -> void
	{
		using namespace components;

		assert(registry.valid(event.wave));

		auto& wave_state = registry.get<wave::WaveState>(event.wave);

		assert(wave_state == wave::WaveState::ENDED);
		constexpr auto new_state = wave::WaveState::DEAD;
		const auto old_state = std::exchange(wave_state, new_state);

		on(registry, StateChanged{.wave = event.wave, .old_state = old_state, .new_state = new_state});
	}

	auto Wave::StateMachine::on(entt::registry& registry, const SkipPreparationRequested& event) noexcept -> void
	{
		using namespace components;

		// SkipPreparationRequested来自用户与UI交互
		// 不能指定wave实体,或者说只有当前波次(实体)才支持该操作
		const auto [wave_current_entity] = registry.ctx().get<const wave::WaveCurrentEntity>();

		assert(registry.valid(wave_current_entity));

		// todo: 需不需要检查当前是否处于准备阶段?理论上不检查也无所谓(但是可以增强鲁棒性)
		{
			const auto state = registry.get<const wave::WaveState>(wave_current_entity);
			assert(state == wave::WaveState::PREPARING);
		}

		// 移除准备时间组件,如此update时会自动触发PreparationEnded事件
		registry.remove<wave::WavePreparationTimer>(wave_current_entity);
	}

	auto Wave::StateMachine::on(entt::registry& registry, const EarlyStartRequested& event) noexcept -> void
	{
		using namespace components;

		// EarlyStartRequested来自用户与UI交互
		// 不能指定wave实体,或者说只有当前波次(实体)才支持该操作
		const auto [wave_current_entity] = registry.ctx().get<const wave::WaveCurrentEntity>();

		// 如果是计时波次则允许提前刷新,否则必须满足条件
		if (const auto& [end_condition] = registry.get<const wave::EndCondition>(wave_current_entity);
			not std::holds_alternative<wave::EndCondition::Duration>(end_condition))
		{
			std::println("非计时波次,无法提前刷新");

			return;
		}

		if (has_next_wave(registry))
		{
			start_new_wave(registry);
		}
	}

	auto Wave::check_end_condition(entt::registry& registry, const entt::entity wave) noexcept -> bool
	{
		using namespace components;

		assert(registry.valid(wave));

		const auto do_check = utility::overloads
		{
				[&](this const auto& self, const wave::EndCondition::Extinction&) noexcept -> bool
				{
					// 1.其他所有波次都已经完成(且被销毁)
					{
						const auto wave_view = registry.view<tags::wave>();

						if (wave_view.size() != 1)
						{
							return false;
						}

						// 唯一的波次就是当前波次
						assert(wave_view.front() == wave);
					}

					// 2.当前波次所有敌人全部死亡(被销毁)
					return self(wave::EndCondition::Clear{});
				},
				[&](const wave::EndCondition::Clear&) noexcept -> bool
				{
					const auto& [wave_enemy] = registry.get<const wave::WaveEnemy>(wave);

					return std::ranges::none_of(
						wave_enemy,
						[&](const entt::entity enemy) noexcept -> bool
						{
							// todo: 每次都检测entity有效?或者是在敌人死亡时移除容器中的敌人实体?
							return registry.valid(enemy);
						}
					);
				},
				[&](this const auto& self, const wave::EndCondition::Duration& condition) noexcept -> bool
				{
					// 如果当前波次所有敌人死亡也视为波次结束
					if (self(wave::EndCondition::Clear{}))
					{
						return true;
					}

					const auto& [elapsed_time] = registry.get<const wave::WaveTimer>(wave);

					return elapsed_time >= condition.duration;
				},
				[&](const wave::EndCondition::Type& condition) noexcept -> bool
				{
					const auto& [wave_enemy] = registry.get<const wave::WaveEnemy>(wave);

					return std::ranges::none_of(
						wave_enemy,
						[&](const entt::entity enemy) noexcept -> bool
						{
							// todo: 每次都检测entity有效?或者是在敌人死亡时移除容器中的敌人实体?
							if (not registry.valid(enemy))
							{
								return false;
							}

							const auto type = registry.get<const combat::Type>(enemy);
							return type == condition.type;
						}
					);
				},
		};

		const auto& [end_condition] = registry.get<const wave::EndCondition>(wave);
		return std::visit(do_check, end_condition);
	}

	auto Wave::has_next_wave(entt::registry& registry) noexcept -> bool
	{
		using namespace components;

		const auto [wave_total_count] = registry.ctx().get<const wave::WaveTotalCount>();
		const auto [wave_current_index] = registry.ctx().get<const wave::WaveCurrentIndex>();

		if (const auto next_wave_index = wave_current_index + 1;
			next_wave_index < wave_total_count)
		{
			return true;
		}

		return false;
	}

	auto Wave::spawn_wave(entt::registry& registry, const components::wave::WaveIndex wave_index) noexcept -> entt::entity
	{
		using namespace components;

		if (const auto [wave_total_count] = registry.ctx().get<const wave::WaveTotalCount>();
			wave_index.index >= wave_total_count)
		{
			std::println("波次 {} 不存在,无法生成!", wave_index.index);
			return entt::null;
		}

		const auto entity = registry.create();

		registry.emplace<wave::WaveIndex>(entity, wave_index);
		{
			const auto& [waves] = registry.ctx().get<const config::wave::Waves>();
			const auto& [spawns, end_condition, preparation_time] = waves[wave_index.index];

			registry.emplace<wave::Wave>(entity, spawns);
			registry.emplace<wave::EndCondition>(entity, end_condition);
			if (preparation_time != sf::Time::Zero)
			{
				registry.emplace<wave::WavePreparationTimer>(entity, preparation_time);
			}

			// 提前扩容好
			auto& [enemy] = registry.emplace<wave::WaveEnemy>(entity);
			enemy.reserve(spawns.size());
		}

		registry.emplace<wave::WaveState>(entity, wave::WaveState::PREPARING);
		registry.emplace<wave::SpawnIndex>(entity, wave::index_type{0});
		registry.emplace<wave::WaveTimer>(entity, sf::Time::Zero);

		registry.emplace<tags::wave>(entity);

		return entity;
	}

	auto Wave::start_from_wave(entt::registry& registry, const components::wave::WaveIndex wave_index) noexcept -> void
	{
		using namespace components;

		// 生成
		const auto entity = spawn_wave(registry, wave_index);
		assert(entity != entt::null);

		registry.ctx().insert_or_assign(wave::WaveCurrentIndex{.index = wave_index.index});
		registry.ctx().insert_or_assign(wave::WaveCurrentEntity{.entity = entity});
	}

	auto Wave::start_new_wave(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		if (has_next_wave(registry))
		{
			const auto [wave_current_index] = registry.ctx().get<const wave::WaveCurrentIndex>();
			const auto next_wave_index = wave_current_index + 1;

			start_from_wave(registry, {next_wave_index});
		}
	}
}
