#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace update
{
	auto weapon(entt::registry& registry, sf::Time delta) noexcept -> void;
}
