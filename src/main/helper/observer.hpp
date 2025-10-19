#pragma once

#include <vector>

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace helper
{
	class Observer
	{
	public:
		class Result
		{
		public:
			// 敌人实体
			entt::entity entity;
			// 距离的平方
			float distance_2;
		};

		// ===============================
		// 获取指定区域内的所有敌人实体

		// 圆形区域
		[[nodiscard]] static auto search_region(entt::registry& registry, entt::entity tower, bool visible_only, sf::Vector2f center, float radius) noexcept -> std::vector<Result>;

		// ===============================
		// 塔寻找攻击目标

		// todo: 假定塔的索敌范围都是圆形
		[[nodiscard]] static auto find_tower_target(entt::registry& registry, entt::entity tower, sf::Vector2f position, float range) noexcept -> entt::entity;

		[[nodiscard]] static auto find_tower_target(entt::registry& registry, entt::entity tower) noexcept -> entt::entity;
	};
}
