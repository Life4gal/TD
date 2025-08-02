#include <systems/update/tower.hpp>

#include <entt/entt.hpp>

namespace systems::update
{
	auto tower(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = delta;
	}
}
