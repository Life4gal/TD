#pragma once

#include <vector>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace game::system::helper
{
	class Observer
	{
	public:
		// =============================
		// 获取范围内的敌人

		// 仅地面
		static auto query_ground(entt::registry& registry, sf::Vector2f world_position, float range) noexcept -> std::vector<entt::entity>;
		// 仅空中
		static auto query_aerial(entt::registry& registry, sf::Vector2f world_position, float range) noexcept -> std::vector<entt::entity>;
		// 两者皆可
		static auto query_dual(entt::registry& registry, sf::Vector2f world_position, float range) noexcept -> std::vector<entt::entity>;
	};
}
