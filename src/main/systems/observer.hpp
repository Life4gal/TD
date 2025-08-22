#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

namespace systems
{
	class Observer
	{
	public:
		// ===================================================

		static auto initialize(entt::registry& registry) noexcept -> void;

		static auto update(entt::registry& registry, sf::Time delta) noexcept -> void;

		// ===================================================

		// 获取指定范围内的敌人

		// 仅地面
		static auto query_ground(entt::registry& registry, sf::Vector2f world_position, float range) noexcept -> std::vector<entt::entity>;
		// 仅空中
		static auto query_aerial(entt::registry& registry, sf::Vector2f world_position, float range) noexcept -> std::vector<entt::entity>;
		// 两者皆可
		static auto query_dual(entt::registry& registry, sf::Vector2f world_position, float range) noexcept -> std::vector<entt::entity>;

		// 仅地面
		static auto query_visible_ground(entt::registry& registry, sf::Vector2f world_position, float range) noexcept -> std::vector<entt::entity>;
		// 仅空中
		static auto query_visible_aerial(entt::registry& registry, sf::Vector2f world_position, float range) noexcept -> std::vector<entt::entity>;
		// 两者皆可
		static auto query_visible_dual(entt::registry& registry, sf::Vector2f world_position, float range) noexcept -> std::vector<entt::entity>;
	};
}
