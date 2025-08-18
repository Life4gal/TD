#pragma once

#include <component/game/entity.hpp>

#include <entt/fwd.hpp>

namespace game::system::helper
{
	class Weapon
	{
	public:
		// =============================
		// 安装武器

		static auto equip(entt::registry& registry, entt::entity tower_entity, component::entity::Type weapon_type) noexcept -> entt::entity;
	};
}
