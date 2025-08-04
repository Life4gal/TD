#pragma once

#include <component/game/entity.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace game::system::helper
{
	class Tower
	{
	public:
		// =============================
		// 建造塔

		static auto build(entt::registry& registry, sf::Vector2u grid_position, component::entity::Type tower_type) noexcept -> entt::entity;
	};
}
