#pragma once

#include <components/entity.hpp>

#include <entt/fwd.hpp>

namespace systems::helper
{
	class Weapon
	{
	public:
		// =============================
		// 安装武器

		static auto equip(entt::registry& registry, entt::entity tower_entity, components::EntityType weapon_type) noexcept -> entt::entity;
	};
}
