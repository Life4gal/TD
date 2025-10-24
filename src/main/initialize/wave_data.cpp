#include <initialize/wave_data.hpp>

#include <algorithm>
#include <chrono>

#include <config/wave.hpp>

#include <components/wave.hpp>

#include <entt/entt.hpp>

namespace
{
	[[nodiscard]] auto load_wave_data() noexcept -> config::wave::Waves
	{
		using namespace config::wave;
		using namespace std::chrono_literals;

		using components::entity::Type;

		Waves waves{};
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

						spawns.resize(10);

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
						end_condition.end_condition = EndCondition::Duration{.duration = 15s};
						// 第一波次无需等待时间(因为需要点击开始)
						preparation_time = sf::Time::Zero;

						break;
					}
					case 1:
					{
						auto& [spawns, end_condition, preparation_time] = wave;

						spawns.resize(10);

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
						end_condition.end_condition = EndCondition::Type{.type = static_cast<Type>(1)};
						// 没有准备时间
						preparation_time = sf::Time::Zero;

						break;
					}
					case 2:
					{
						auto& [spawns, end_condition, preparation_time] = wave;

						spawns.resize(12);

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
						end_condition.end_condition = EndCondition::Type{.type = static_cast<Type>(2)};
						// 15秒准备时间
						preparation_time = sf::seconds(15);

						break;
					}
					case 3:
					{
						auto& [_, end_condition, preparation_time] = wave;

						// 该波次什么都不做,只是等待所有波次所有敌人全部死亡
						// todo: 如此设计并不好,因为这本质上是一个不存在的波次
						end_condition.end_condition = EndCondition::Extinction{};
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

namespace initialize
{
	auto wave_data(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto config = load_wave_data();
		// 按照delay排序
		std::ranges::for_each(
			config.waves,
			[](auto& spawns) noexcept -> void
			{
				std::ranges::sort(spawns, std::ranges::less{}, &config::wave::Spawn::delay);
			},
			&config::wave::Wave::spawns
		);

		// 总的波次数量
		registry.ctx().emplace<wave::WaveTotalCount>(wave::WaveTotalCount{.count = static_cast<wave::index_type>(config.waves.size())});
		// 当前生成的波次
		registry.ctx().emplace<wave::WaveCurrentIndex>(wave::WaveCurrentIndex{.index = wave::wave_not_start});
		// 当前生成的波次实体
		registry.ctx().emplace<wave::WaveCurrentEntity>(wave::WaveCurrentEntity{.entity = entt::null});

		// 所有波次数据
		registry.ctx().emplace<config::wave::Waves>(std::move(config));
	}
}
