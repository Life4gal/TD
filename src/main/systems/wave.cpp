#include <systems/wave.hpp>

#include <components/tags.hpp>

#include <helper/enemy.hpp>
#include <helper/wave.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Wave::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		// =============================
		// 1.创建波次
		// =============================

		if (auto& [wave_duration] = registry.ctx().get<wave::WaveDuration>();
			wave_duration < delta)
		{
			if (helper::Wave::has_next_wave(registry))
			{
				helper::Wave::start(registry);
			}
		}
		else
		{
			wave_duration -= delta;
		}

		// =============================
		// 2.生成波次
		// =============================

		for (const auto wave_view = registry.view<wave::Wave, wave::WaveSpawnIndex, wave::WaveTimer>();
		     const auto [entity, wave, index, timer]: wave_view.each())
		{
			// =============================
			// 1.更新波次计时器
			// =============================

			const auto last_wave_timer = timer.timer;
			timer.timer += delta;

			// =============================
			// 2.检查是否需要生成敌人
			// =============================

			const auto index_value = std::to_underlying(index);

			const auto to_spawn =
					wave.spawns |
					// 排除已经生成的部分
					std::views::drop(index_value) |
					// 处于时间范围内
					std::views::take_while(
						[&](const wave::Spawn spawn) noexcept -> bool
						{
							const auto delay = spawn.delay;

							return delay > last_wave_timer and delay < timer.timer;
						}
					);

			if (not to_spawn.empty())
			{
				std::ranges::for_each(
					to_spawn,
					[&](const wave::Spawn& spawn) noexcept -> void
					{
						helper::Enemy::spawn(registry, spawn.gate_id, spawn.type);
					}
				);

				index = static_cast<wave::WaveSpawnIndex>(index_value + std::ranges::distance(to_spawn));
			}

			// =============================
			// 3.检查是否生成了所有敌人
			// =============================

			if (std::to_underlying(index) == wave.spawns.size())
			{
				// 标记死亡即可,墓地会销毁所有标记了死亡的实体
				// todo: Wave系统每个模拟帧都会更新,但是Graveyard系统每帧只更新一次,所以这里标记死亡的实体可能会还未销毁(如果模拟帧数>1)
				registry.emplace<tags::dead>(entity);

				// todo: 结束一个波次有奖励吗?
			}
		}
	}
}
