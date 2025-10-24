#include <helper/enemy.hpp>

#include <print>

#include <components/core/tags.hpp>
#include <components/combat/unit.hpp>
#include <components/combat/enemy.hpp>

#include <utility/time.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Enemy::reach(entt::registry& registry, const entt::entity enemy) noexcept -> void
	{
		using namespace components;

		assert(registry.all_of<tags::enemy>(enemy));

		registry.emplace<tags::dead>(enemy);
		registry.emplace<tags::cod_reached>(enemy);
	}

	auto Enemy::kill(entt::registry& registry, const entt::entity attacker, const entt::entity victim) noexcept -> void
	{
		using namespace components;

		assert(registry.valid(attacker));
		assert(registry.valid(victim));
		assert(registry.all_of<tags::enemy>(victim));

		const auto& [attacker_name] = registry.get<const combat::Name>(attacker);
		const auto& [victim_name] = registry.get<const combat::Name>(victim);

		// 如果目标已经死亡则什么也不做
		if (registry.all_of<tags::dead>(victim))
		{
			std::println(
				"[{:%Y-%m-%d %H:%M:%S}] [{}](EID:{})试图击杀[{}](EID:{}),但是其已死亡",
				utility::zoned_now(),
				attacker_name,
				std::to_underlying(attacker),
				victim_name,
				std::to_underlying(victim)
			);
			return;
		}

		std::println(
			"[{:%Y-%m-%d %H:%M:%S}] [{}](EID:{})击杀[{}](EID:{})",
			utility::zoned_now(),
			attacker_name,
			std::to_underlying(attacker),
			victim_name,
			std::to_underlying(victim)
		);

		registry.emplace<tags::dead>(victim);
		registry.emplace<tags::cod_killed>(victim);
	}

	auto Enemy::hurt(entt::registry& registry, const entt::entity attacker, const entt::entity victim, const float damage) noexcept -> void
	{
		using namespace components;

		assert(registry.valid(attacker));
		assert(registry.valid(victim));
		assert(registry.all_of<tags::enemy>(victim));

		const auto& [attacker_name] = registry.get<const combat::Name>(attacker);
		const auto& [victim_name] = registry.get<const combat::Name>(victim);

		// 如果目标已经死亡则什么也不做
		if (registry.all_of<tags::dead>(victim))
		{
			std::println(
				"[{:%Y-%m-%d %H:%M:%S}] [{}](EID:{})试图伤害[{}](EID:{}),但是其已死亡",
				utility::zoned_now(),
				attacker_name,
				std::to_underlying(attacker),
				victim_name,
				std::to_underlying(victim)
			);
			return;
		}

		auto& [health] = registry.get<enemy::Health>(victim);

		const auto new_health = health - damage;
		const auto old_health = std::exchange(health, new_health);

		std::println(
			"[{:%Y-%m-%d %H:%M:%S}] [{}](EID:{})对[{}](EID:{})造成了{:.3f}点伤害({:.3f} ==> {:.3f})",
			utility::zoned_now(),
			attacker_name,
			std::to_underlying(attacker),
			victim_name,
			std::to_underlying(victim),
			damage,
			old_health,
			new_health
		);

		if (new_health <= 0)
		{
			kill(registry, attacker, victim);
		}
	}
}
