#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace systems::update
{
	auto enemy(entt::registry& registry, sf::Time delta) noexcept -> void;
}
