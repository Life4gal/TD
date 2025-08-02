#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace systems::helper
{
	class Player
	{
	public:
		// =============================
		// 建造/销毁塔

		static auto try_build_tower(entt::registry& registry, sf::Vector2f world_position) noexcept -> bool;

		static auto try_destroy_tower(entt::registry& registry, sf::Vector2f world_position) noexcept -> bool;

		// =============================
		// 安装/解除武器

		static auto try_equip_weapon(entt::registry& registry, sf::Vector2f world_position) noexcept -> bool;

		static auto try_remove_weapon(entt::registry& registry, sf::Vector2f world_position) noexcept -> bool;
	};
}
