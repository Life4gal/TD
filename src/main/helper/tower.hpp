#pragma once

#include <components/entity.hpp>

#include <entt/fwd.hpp>

namespace helper
{
	class Tower
	{
	public:
		// 构建塔(实体组件),不负责设置塔位置(同时也不设置塔标记)
		static auto build(entt::registry& registry, components::entity::Type type) noexcept -> entt::entity;
	};
}
