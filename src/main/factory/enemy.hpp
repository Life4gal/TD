#pragma once

#include <components/combat/unit.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace factory
{
	// 生成敌人(指定位置)
	auto enemy(entt::registry& registry, sf::Vector2u point, components::combat::Type type) noexcept -> entt::entity;

	// 生成敌人(指定出生点)
	auto enemy(entt::registry& registry, std::uint32_t start_gate_id, components::combat::Type type) noexcept -> entt::entity;
}
