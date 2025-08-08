#include <system/game/helper/wave.hpp>

#include <algorithm>
#include <ranges>
#include <print>

#include <component/game/tags.hpp>
#include <component/game/wave.hpp>

#include <entt/entt.hpp>

namespace game::system::helper
{
	auto Wave::load(entt::registry& registry) noexcept -> void
	{
		using namespace component;
		using namespace std::chrono_literals;

		wave::WaveSequence wave_sequence{};
		{
			auto& [waves, durations] = wave_sequence;

			waves.resize(3);
			durations.resize(3);

			for (auto [wave, duration]: std::views::zip(waves, durations))
			{
				// wave
				{
					auto& [spawns] = wave;

					spawns.resize(10);

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

	auto Wave::has_next_wave(entt::registry& registry) noexcept -> bool
	{
		using namespace component;

		const auto& [waves, durations] = registry.ctx().get<const wave::WaveSequence>();
		const auto next_wave_index = registry.ctx().get<wave::WaveIndex>();

		return std::to_underlying(next_wave_index) < waves.size();
	}

	auto Wave::spawn(entt::registry& registry, const component::wave::WaveIndex wave_index) noexcept -> void
	{
		using namespace component;

		const auto& [waves, durations] = registry.ctx().get<const wave::WaveSequence>();

		const auto wave_index_value = std::to_underlying(wave_index);
		if (wave_index_value >= waves.size())
		{
			std::println("波次 {} 不存在, 无法生成!", wave_index_value);
			return;
		}

		std::println("[{}] 生成波次 {}", std::chrono::system_clock::now(), wave_index_value);

		const auto entity = registry.create();

		registry.emplace<wave::Wave>(entity, waves[wave_index_value]);
		registry.emplace<wave::WaveSpawnIndex>(entity, static_cast<wave::WaveSpawnIndex>(0));
		registry.emplace<wave::WaveTimer>(entity);

		registry.emplace<tags::wave::identifier>(entity);
	}

	auto Wave::start_at(entt::registry& registry, const component::wave::WaveIndex wave_index) noexcept -> void
	{
		using namespace component;

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
		using namespace component;

		if (has_next_wave(registry))
		{
			const auto next_wave_index = registry.ctx().get<wave::WaveIndex>();
			start_at(registry, next_wave_index);
		}
	}
}
