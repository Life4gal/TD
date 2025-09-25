#include <systems/timed_life.hpp>

#include <components/timed_life.hpp>

#include <entt/entt.hpp>

namespace systems
{
	auto TimedLife::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		for (const auto timed_life_view = registry.view<components::TimedLife>();
		     const auto [entity, timed_life]: timed_life_view.each())
		{
			timed_life.elapsed += delta;

			if (timed_life.elapsed >= timed_life.total)
			{
				registry.destroy(entity);
			}
		}
	}
}
