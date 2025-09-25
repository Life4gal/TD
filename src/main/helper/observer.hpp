#pragma once

#include <vector>

#include <components/enemy.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace helper
{
	class Observer
	{
	public:
		// 获取指定范围内的敌人

		// AABB
		static auto query(
			entt::registry& registry,
			components::enemy::Archetype archetype,
			bool visible_only,
			const sf::FloatRect& bounds
		) noexcept -> std::vector<entt::entity>;

		// CIRCLE
		static auto query(
			entt::registry& registry,
			components::enemy::Archetype archetype,
			bool visible_only,
			sf::Vector2f center,
			float radius
		) noexcept -> std::vector<entt::entity>;

		// OBB
		static auto query(
			entt::registry& registry,
			components::enemy::Archetype archetype,
			bool visible_only,
			const sf::FloatRect& bounds,
			sf::Angle angle
		) noexcept -> std::vector<entt::entity>;

		// SECTOR
		static auto query(
			entt::registry& registry,
			components::enemy::Archetype archetype,
			bool visible_only,
			sf::Vector2f center,
			float radius,
			sf::Angle from,
			sf::Angle to
		) noexcept -> std::vector<entt::entity>;
	};
}
