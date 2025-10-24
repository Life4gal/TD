#pragma once

#include <components/combat/enemy.hpp>

#include <entt/fwd.hpp>

namespace helper
{
	class Tower
	{
	public:
		// 获取塔的索敌类型
		[[nodiscard]] static auto targeting_of(const entt::registry& registry, entt::entity tower) noexcept -> components::enemy::Archetype;
	};
}
