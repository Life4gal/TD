#include <helper/wave.hpp>

#include <algorithm>
#include <ranges>
#include <print>

#include <components/tags.hpp>

#include <entt/entt.hpp>

namespace
{
	// todo: 载入配置文件
	[[nodiscard]] auto make_waves_data() noexcept -> components::wave::WaveSequence
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

					// std::ranges::sort(spawns, std::ranges::less{}, &wave::Spawn::delay);
				}
				// duration
				{
					duration.duration = 10s;
				}
			}
		}

		return wave_sequence;
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
				std::ranges::sort(spawns, std::ranges::less{}, &wave::Spawn::delay);
			},
			&wave::Wave::spawns
		);

		// 一秒后开始生成
		registry.ctx().emplace<wave::WaveDuration>(1s);
		// 从第一波开始
		registry.ctx().emplace<wave::WaveIndex>(static_cast<wave::WaveIndex>(0));
		registry.ctx().emplace<wave::WaveSequence>(std::move(waves));
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
