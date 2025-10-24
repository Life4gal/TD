#include <update/wave.hpp>

#include <helper/entity.hpp>
#include <helper/enemy.hpp>
#include <helper/wave.hpp>

#include <entt/entt.hpp>

namespace update
{
	auto wave(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		// 更新波次
		for (const auto wave_view = registry.view<const wave::WaveState, wave::SpawnIndex, wave::WaveTimer>();
		     const auto [entity, state, spawn_index, wave_timer]: wave_view.each())
		{
			using wave::WaveState;
			using sm = helper::Wave::StateMachine;
			switch (state)
			{
				case WaveState::PREPARING:
				{
					auto* preparation_timer = registry.try_get<wave::WavePreparationTimer>(entity);

					if (preparation_timer == nullptr)
					{
						// 没有准备时间,直接结束准备阶段
						sm::on(registry, sm::PreparationEnded{.wave = entity});

						break;
					}

					preparation_timer->remaining -= delta;
					if (preparation_timer->remaining <= sf::Time::Zero)
					{
						// 准备阶段结束
						sm::on(registry, sm::PreparationEnded{.wave = entity});

						// 直接移除准备时间组件(其实也可以不移除)
						registry.remove<wave::WavePreparationTimer>(entity);
					}

					break;
				}
				case WaveState::SPAWNING:
				{
					// 当前波次数据
					const auto& [spawns] = registry.get<const wave::Wave>(entity);

					const auto last_spawn_time = wave_timer.elapsed_time;
					wave_timer.elapsed_time += delta;
					const auto current_spawn_time = wave_timer.elapsed_time;

					// 生成敌人
					const auto to_spawn =
							spawns |
							// 排除已经生成的部分
							std::views::drop(spawn_index.index) |
							// 处于时间范围内
							std::views::take_while(
								[&](const config::wave::Spawn& spawn) noexcept -> bool
								{
									const auto delay = spawn.delay;

									return last_spawn_time <= delay and delay < current_spawn_time;
								}
							);

					if (not to_spawn.empty())
					{
						// 要记录所有生成的敌人实体
						// todo: 目前没有处理死亡的敌人
						auto& [wave_enemy] = registry.get<wave::WaveEnemy>(entity);

						std::ranges::for_each(
							to_spawn,
							[&](const config::wave::Spawn& spawn) noexcept -> void
							{
								const auto enemy = helper::Enemy::spawn(registry, spawn.gate_id, spawn.type);
								wave_enemy.push_back(enemy);
							}
						);

						spawn_index.index += static_cast<wave::index_type>(std::ranges::distance(to_spawn));
					}

					if (spawn_index.index == spawns.size())
					{
						// 生成完毕
						sm::on(registry, sm::SpawnsCompleted{.wave = entity});
					}

					break;
				}
				case WaveState::RUNNING:
				{
					wave_timer.elapsed_time += delta;

					// 检查是否满足结束条件
					if (helper::Wave::check_end_condition(registry, entity))
					{
						sm::on(registry, sm::ConditionMet{.wave = entity});
					}

					break;
				}
				case WaveState::COMPLETED:
				{
					// 结束波次
					sm::on(registry, sm::Completed{.wave = entity});

					break;
				}
				case WaveState::ENDED:
				{
					// 检测该波次是否还有未死亡(未被销毁)的敌人
					if (const auto& [wave_enemy] = registry.get<const wave::WaveEnemy>(entity);
						std::ranges::none_of(
							wave_enemy,
							[&](const entt::entity enemy) noexcept -> bool
							{
								return registry.valid(enemy);
							}
						))
					{
						sm::on(registry, sm::Finished{.wave = entity});
					}

					break;
				}
				case WaveState::DEAD:
				{
					// 销毁波次实体
					helper::Entity::kill(registry, entity);

					break;
				}
			}
		}
	}
}
