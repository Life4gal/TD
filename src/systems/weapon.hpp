#pragma once

#include <components/entity.hpp>

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems
{
	class Weapon
	{
	public:
		static auto initialize(entt::registry& registry) noexcept -> void;

		static auto update(entt::registry& registry, sf::Time delta) noexcept -> void;

		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;

		// =============================
		// 安装武器

		static auto equip(entt::registry& registry, entt::entity tower_entity, components::EntityType weapon_type) noexcept -> entt::entity;
	};
}
