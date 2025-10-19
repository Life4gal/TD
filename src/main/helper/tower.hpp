#pragma once

#include <components/entity.hpp>
#include <components/enemy.hpp>

#include <entt/fwd.hpp>

namespace helper
{
	class Tower
	{
	public:
		// 构建塔(实体组件),不负责设置塔位置(同时也不设置塔标记)
		static auto build(entt::registry& registry, components::entity::Type type) noexcept -> entt::entity;

		// 获取塔的索敌类型
		[[nodiscard]] static auto targeting_of(const entt::registry& registry, entt::entity tower) noexcept -> components::enemy::Archetype;
	};
}
