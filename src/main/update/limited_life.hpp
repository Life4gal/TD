#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace update
{
	auto limited_life(entt::registry& registry, sf::Time delta) noexcept -> void;
}
