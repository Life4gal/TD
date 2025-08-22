#include <systems/wave.hpp>

#include <print>

#include <components/tags.hpp>

#include <systems/enemy.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Wave::load(entt::registry& registry) noexcept -> void
	{
		using namespace components;
		using namespace std::chrono_literals;

		wave::WaveSequence wave_sequence{};
		{
			wave_sequence.waves.resize(3);
			wave_sequence.durations.resize(3);

			for (auto [wave, duration]: std::views::zip(wave_sequence.waves, wave_sequence.durations))
			{
				// wave
				{
					auto& spawns = wave.spawns;

					wave.spawns.resize(10);

					spawns[0] = {.type = static_cast<entity::Type>(0), .gate_id = 0, .delay = 500ms};
					spawns[1] = {.type = static_cast<entity::Type>(0), .gate_id = 0, .delay = 1500ms};
					spawns[2] = {.type = static_cast<entity::Type>(0), .gate_id = 0, .delay = 2500ms};
					spawns[3] = {.type = static_cast<entity::Type>(0), .gate_id = 0, .delay = 3500ms};
					spawns[4] = {.type = static_cast<entity::Type>(0), .gate_id = 0, .delay = 4500ms};

					spawns[5] = {.type = static_cast<entity::Type>(0), .gate_id = 1, .delay = 500ms};
					spawns[6] = {.type = static_cast<entity::Type>(0), .gate_id = 1, .delay = 1500ms};
					spawns[7] = {.type = static_cast<entity::Type>(0), .gate_id = 1, .delay = 2500ms};
					spawns[8] = {.type = static_cast<entity::Type>(0), .gate_id = 1, .delay = 3500ms};
					spawns[9] = {.type = static_cast<entity::Type>(0), .gate_id = 1, .delay = 4500ms};

					std::ranges::sort(spawns, std::ranges::less{}, &wave::Spawn::delay);
				}
				// duration
				{
					duration.duration = 10s;
				}
			}
		}

		// 一秒后开始生成
		registry.ctx().emplace<wave::WaveDuration>(1s);
		// 从第一波开始
		registry.ctx().emplace<wave::WaveIndex>(static_cast<wave::WaveIndex>(0));
		registry.ctx().emplace<wave::WaveSequence>(std::move(wave_sequence));
	}

	auto Wave::initialize(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::wave>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto& [spawns] = reg.get<const wave::Wave>(entity);
				const auto wave_index = reg.get<const wave::WaveIndex>(entity);

				std::println(
					"[{}] 生成波次{},共{}个敌人({})",
					std::chrono::system_clock::now(),
					std::to_underlying(wave_index),
					spawns.size(),
					std::to_underlying(entity)
				);
			}>();

		registry.on_destroy<tags::wave>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] 结束波次({})", std::chrono::system_clock::now(), std::to_underlying(entity));
			}>();
	}

	auto Wave::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		// =============================
		// 1.创建波次
		// =============================

		if (auto& [wave_duration] = registry.ctx().get<wave::WaveDuration>();
			wave_duration < delta)
		{
			if (has_next_wave(registry))
			{
				start(registry);
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
						Enemy::spawn(registry, spawn.gate_id, spawn.type);
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
				registry.emplace<tags::dead>(entity);

				// todo: 结束一个波次有奖励吗?
			}
		}
	}

	auto Wave::has_next_wave(entt::registry& registry) noexcept -> bool
	{
		using namespace components;

		const auto& [waves, durations] = registry.ctx().get<const wave::WaveSequence>();
		const auto next_wave_index = registry.ctx().get<wave::WaveIndex>();

		return std::to_underlying(next_wave_index) < waves.size();
	}

	auto Wave::spawn(entt::registry& registry, const components::wave::WaveIndex wave_index) noexcept -> void
	{
		using namespace components;

		const auto& [waves, durations] = registry.ctx().get<const wave::WaveSequence>();

		const auto wave_index_value = std::to_underlying(wave_index);
		if (wave_index_value >= waves.size())
		{
			std::println("波次 {} 不存在,无法生成!", wave_index_value);
			return;
		}

		const auto entity = registry.create();

		registry.emplace<wave::Wave>(entity, waves[wave_index_value]);
		registry.emplace<wave::WaveSpawnIndex>(entity, static_cast<wave::WaveSpawnIndex>(0));
		registry.emplace<wave::WaveTimer>(entity);
		registry.emplace<wave::WaveIndex>(entity, wave_index);

		registry.emplace<tags::wave>(entity);
	}

	auto Wave::start_at(entt::registry& registry, const components::wave::WaveIndex wave_index) noexcept -> void
	{
		using namespace components;

		const auto& [waves, durations] = registry.ctx().get<const wave::WaveSequence>();

		const auto wave_index_value = std::to_underlying(wave_index);

		// 生成
		spawn(registry, wave_index);

		auto& wave_duration = registry.ctx().get<wave::WaveDuration>();
		auto& next_wave_index = registry.ctx().get<wave::WaveIndex>();

		const auto next_wave_index_value = wave_index_value + 1;
		// 设定下一个波次
		next_wave_index = static_cast<wave::WaveIndex>(next_wave_index_value);

		// 如果还有下一波次
		if (next_wave_index_value < waves.size())
		{
			// 设定持续时间
			wave_duration = durations[wave_index_value];
		}
		else
		{
			wave_duration.duration = std::chrono::hours{9999};
		}
	}

	auto Wave::start(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		if (has_next_wave(registry))
		{
			const auto next_wave_index = registry.ctx().get<wave::WaveIndex>();
			start_at(registry, next_wave_index);
		}
	}
}
