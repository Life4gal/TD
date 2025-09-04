#pragma once

#include <components/entity.hpp>

#include <entt/fwd.hpp>

namespace helper
{
	class Tower
	{
	public:
		static auto build(entt::registry& registry, components::entity::Type type) noexcept -> entt::entity;
	};
}
