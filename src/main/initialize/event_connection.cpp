#include <initialize/event_connection.hpp>

#include <print>

#include <components/core/tags.hpp>
#include <components/core/transform.hpp>
#include <components/combat/unit.hpp>
#include <components/game/wave.hpp>

#include <utility/time.hpp>

#include <entt/entt.hpp>

namespace
{
	auto connect_wave(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::wave>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [wave_index] = reg.get<const wave::WaveIndex>(entity);
				const auto& [spawns] = reg.get<const wave::Wave>(entity);

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 生成波次{},共{}个敌人(EID:{})",
					utility::zoned_now(),
					wave_index,
					spawns.size(),
					entt::to_integral(entity)
				);
			}>();

		registry.on_destroy<tags::wave>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 结束波次(EID:{})",
					utility::zoned_now(),
					entt::to_integral(entity)
				);
			}>();
	}

	auto connect_tower(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto type = reg.get<const combat::Type>(entity);
				const auto [position] = reg.get<const transform::Position>(entity);
				const auto& [name] = reg.get<const combat::Name>(entity);

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 在({:.0f}:{:.0f})建造[0x{:08x}]型塔[{}](EID:{})",
					utility::zoned_now(),
					position.x,
					position.y,
					std::to_underlying(type),
					name,
					std::to_underlying(entity)
				);
			}>();

		registry.on_destroy<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 销毁塔(EID:{})",
					utility::zoned_now(),
					std::to_underlying(entity)
				);
			}>();
	}

	auto connect_enemy(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto type = reg.get<const combat::Type>(entity);
				const auto [position] = reg.get<const transform::Position>(entity);
				const auto& [name] = reg.get<const combat::Name>(entity);

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 在({:.0f}:{:.0f})生成[0x{:08x}]型敌人[{}](EID:{})",
					utility::zoned_now(),
					position.x,
					position.y,
					std::to_underlying(type),
					name,
					std::to_underlying(entity)
				);
			}>();

		registry.on_destroy<tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 销毁敌人(EID:{})",
					utility::zoned_now(),
					std::to_underlying(entity)
				);
			}>();
	}
}

namespace initialize
{
	auto event_connection(entt::registry& registry) noexcept -> void
	{
		connect_wave(registry);
		connect_tower(registry);
		connect_enemy(registry);
	}
}
