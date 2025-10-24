#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace update
{
	auto wave(entt::registry& registry, sf::Time delta) noexcept -> void;
}
