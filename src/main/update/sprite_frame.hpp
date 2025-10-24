#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace update
{
	auto sprite_frame(entt::registry& registry, sf::Time delta) noexcept -> void;
}
