#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace game::system::update
{
	auto observer(entt::registry& registry, sf::Time delta) noexcept -> void;
}
