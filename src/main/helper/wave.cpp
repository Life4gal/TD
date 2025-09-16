#include <helper/wave.hpp>

#include <algorithm>
#include <ranges>
#include <print>

#include <components/tags.hpp>

#include <utility/functional.hpp>
#include <utility/time.hpp>

#include <meta/enumeration.hpp>

#include <entt/entt.hpp>

namespace
{
	// todo: 载入配置文件
	[[nodiscard]] auto make_waves_data() noexcept -> config::wave::Waves
	{
		using namespace config;
		using namespace std::chrono_literals;

		using components::entity::Type;

		wave::Waves waves{};
		{
			// 4个波次
			waves.waves.resize(4);

			for (auto [index, wave]: waves.waves | std::views::enumerate)
			{
				switch (index % 4)
				{
					case 0:
					{
						auto& [spawns, end_condition, preparation_time] = wave;

						wave.spawns.resize(10);

						spawns[0] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 500ms};
						spawns[1] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 1500ms};
						spawns[2] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 2500ms};
						spawns[3] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 3500ms};
						spawns[4] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 4500ms};

						spawns[5] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 500ms};
						spawns[6] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 1500ms};
						spawns[7] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 2500ms};
						spawns[8] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 3500ms};
						spawns[9] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 4500ms};

						// std::ranges::sort(spawns, std::ranges::less{}, &wave::Spawn::delay);

						// 15秒后直接结束
						end_condition.end_condition = wave::EndCondition::Duration{.duration = 15s};
						// 第一波次无需等待时间(因为需要点击开始)
						preparation_time = sf::Time::Zero;

						break;
					}
					case 1:
					{
						auto& [spawns, end_condition, preparation_time] = wave;

						wave.spawns.resize(10);

						spawns[0] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 500ms};
						spawns[1] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 1500ms};
						spawns[2] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 2500ms};
						spawns[3] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 3500ms};
						spawns[4] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 4500ms};

						spawns[5] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 500ms};
						spawns[6] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 1500ms};
						spawns[7] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 2500ms};
						spawns[8] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 3500ms};
						spawns[9] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 4500ms};

						// std::ranges::sort(spawns, std::ranges::less{}, &wave::Spawn::delay);

						// 所有type为1的敌人全部死亡
						end_condition.end_condition = wave::EndCondition::Type{.type = static_cast<Type>(1)};
						// 没有准备时间
						preparation_time = sf::Time::Zero;

						break;
					}
					case 2:
					{
						auto& [spawns, end_condition, preparation_time] = wave;

						wave.spawns.resize(12);

						spawns[0] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 500ms};
						spawns[1] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 1500ms};
						spawns[2] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 2500ms};
						spawns[3] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 3500ms};
						spawns[4] = {.type = static_cast<Type>(0), .gate_id = 0, .delay = 4500ms};

						spawns[5] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 500ms};
						spawns[6] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 1500ms};
						spawns[7] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 2500ms};
						spawns[8] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 3500ms};
						spawns[9] = {.type = static_cast<Type>(1), .gate_id = 1, .delay = 4500ms};

						// BOSS
						spawns[10] = {.type = static_cast<Type>(2), .gate_id = 0, .delay = 5000ms};
						spawns[11] = {.type = static_cast<Type>(2), .gate_id = 1, .delay = 5000ms};

						// std::ranges::sort(spawns, std::ranges::less{}, &wave::Spawn::delay);

						// 所有type为2的敌人(BOSS)全部死亡
						end_condition.end_condition = wave::EndCondition::Type{.type = static_cast<Type>(2)};
						// 15秒准备时间
						preparation_time = sf::seconds(15);

						break;
					}
					case 3:
					{
						auto& [_, end_condition, preparation_time] = wave;

						// 该波次什么都不做,只是等待所有波次所有敌人全部死亡
						// todo: 如此设计并不好,因为这本质上是一个不存在的波次
						end_condition.end_condition = wave::EndCondition::Extinction{};
						preparation_time = sf::Time::Zero;

						break;
					}
					default:
					{
						std::unreachable();
					}
				}
			}
		}

		return waves;
	}
}

namespace helper
{
	auto Wave::load(entt::registry& registry) noexcept -> void
	{
		using namespace components;
		using namespace std::chrono_literals;

		// 读取文件
		auto waves = make_waves_data();
		// 按照delay排序
		std::ranges::for_each(
			waves.waves,
			[](auto& spawns) noexcept -> void
			{
				std::ranges::sort(spawns, std::ranges::less{}, &config::wave::Spawn::delay);
			},
			&config::wave::Wave::spawns
		);

		// 总的波次数量
		registry.ctx().emplace<wave::WaveTotalCount>(wave::WaveTotalCount{.count = static_cast<wave::index_type>(waves.waves.size())});
		// 当前生成的波次
		registry.ctx().emplace<wave::WaveCurrentIndex>(wave::WaveCurrentIndex{.index = wave::wave_not_start});
		// 当前生成的波次实体
		registry.ctx().emplace<wave::WaveCurrentEntity>(wave::WaveCurrentEntity{.entity = entt::null});

		// 所有波次数据
		registry.ctx().emplace<config::wave::Waves>(std::move(waves));
	}

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

							const auto type = registry.get<const entity::Type>(enemy);
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

			registry.emplace<wave::Wave>(entity, std::cref(spawns));
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
