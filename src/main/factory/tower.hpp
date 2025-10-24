#pragma once

#include <components/combat/unit.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace factory
{
	// 建造塔(指定位置)
	auto tower(entt::registry& registry, sf::Vector2u point, components::combat::Type type) noexcept -> entt::entity;
}
