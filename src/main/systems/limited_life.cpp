#include <systems/limited_life.hpp>

#include <components/entity.hpp>
#include <components/limited_life.hpp>

#include <helper/entity.hpp>

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

			helper::Entity::destroy(registry, entity);
		}
	}

	auto update_distance(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		for (const auto travel_view = registry.view<limited_life::Distance, entity::Position>();
		     const auto [entity, travel, position]: travel_view.each())
		{
			const auto dp = position.position - travel.initial_position;

			if (const auto distance_2 = dp.lengthSquared();
				distance_2 < travel.max_distance_2)
			{
				continue;
			}

			helper::Entity::destroy(registry, entity);
		}
	}
}

namespace systems
{
	auto LimitedLife::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		update_time(registry, delta);
		update_distance(registry);
	}
}
