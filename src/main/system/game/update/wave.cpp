#include <system/game/update/wave.hpp>

#include <algorithm>
#include <ranges>

#include <component/game/tags.hpp>
#include <component/game/wave.hpp>

#include <system/game/helper/enemy.hpp>
#include <system/game/helper/wave.hpp>

#include <entt/entt.hpp>

namespace game::system::update
{
	auto wave(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace component;

		// =================================
		// 1.创建波次
		// =================================

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

		// =================================
		// 2.生成波次
		// =================================

		for (const auto wave_view = registry.view<tags::wave::identifier, wave::Wave, wave::WaveSpawnIndex, wave::WaveTimer>();
		     const auto [entity, wave, index, timer]: wave_view.each())
		{
			// ===============
			// 1.更新波次计时器
			// ===============

			const auto last_wave_timer = timer.timer;
			timer.timer += delta;

			// ===============
			// 2.检查是否需要生成敌人
			// ===============

			{
				const auto index_value = std::to_underlying(index);

				const auto to_spawn =
						wave.spawns |
						// 排除已生成的部分
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
			}

			// ===============
			// 3.检查是否生成了所有敌人
			// ===============

			if (std::to_underlying(index) == wave.spawns.size())
			{
				// 标记该波次生成完毕
				registry.emplace<tags::wave::status::end>(entity);
			}
		}

		// =================================
		// 3.移除结束的波次
		// =================================

		{
			const auto wave_view = registry.view<tags::wave::identifier, tags::wave::status::end>();

			// todo: 结束一个波次有奖励吗?

			registry.destroy(wave_view.begin(), wave_view.end());
		}
	}
}
