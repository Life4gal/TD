#include <update/limited_life.hpp>

#include <components/core/transform.hpp>
#include <components/combat/limited_life.hpp>

#include <helper/combat_unit.hpp>

#include <entt/entt.hpp>

namespace
{
	auto update_time(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		for (const auto time_view = registry.view<limited_life::Time>();
		     const auto [entity, remaining]: time_view.each())
		{
			remaining.remaining -= delta;
			if (remaining.remaining > sf::Time::Zero)
			{
				continue;
			}

			// todo: 只有combat_unit会用到limited_life吗?
			helper::CombatUnit::destroy(registry, entity);
		}
	}

	auto update_distance(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		for (const auto travel_view = registry.view<limited_life::Distance, transform::Position>();
		     const auto [entity, travel, position]: travel_view.each())
		{
			const auto dp = position.position - travel.initial_position;

			if (const auto distance_2 = dp.lengthSquared();
				distance_2 < travel.max_distance_2)
			{
				continue;
			}

			// todo: 只有combat_unit会用到limited_life吗?
			helper::CombatUnit::destroy(registry, entity);
		}
	}
}

namespace update
{
	auto limited_life(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		update_time(registry, delta);
		update_distance(registry);
	}
}
