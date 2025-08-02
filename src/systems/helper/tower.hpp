#pragma once

#include <components/entity.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace systems::helper
{
	class Tower
	{
	public:
		// =============================
		// 建造塔

		static auto build(entt::registry& registry, sf::Vector2u grid_position, components::EntityType tower_type) noexcept -> entt::entity;
	};
}
