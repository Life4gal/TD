#include <systems/update/enemy.hpp>

#include <entt/entt.hpp>

namespace systems::update
{
	auto enemy(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = delta;
	}
}
