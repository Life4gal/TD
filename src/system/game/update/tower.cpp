#include <system/game/update/tower.hpp>

#include <entt/entt.hpp>

namespace game::system::update
{
	auto tower(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = delta;
	}
}
