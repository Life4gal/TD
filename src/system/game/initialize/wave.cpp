#include <system/game/initialize/wave.hpp>

#include <chrono>
#include <print>

#include <component/game/tags.hpp>
#include <component/game/wave.hpp>

#include <entt/entt.hpp>

namespace game::system::initialize
{
	auto wave(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		registry.on_construct<tags::wave::identifier>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto& [spawns] = reg.get<const wave::Wave>(entity);
				const auto wave_index = reg.get<const wave::WaveIndex>(entity);

				std::println(
					"[{}] 生成波次{}(第 {} 波, 共 {} 个敌人)",
					std::chrono::system_clock::now(),
					std::to_underlying(entity),
					std::to_underlying(wave_index),
					spawns.size()
				);
			}>();

		registry.on_destroy<tags::wave::identifier>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] 结束波次{}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}>();
	}
}
