#pragma once

#include <unordered_map>

#include <components/entity.hpp>

#include <entt/entity/entity.hpp>

#include <SFML/System/Vector2.hpp>

namespace components
{
	class PlayerData
	{
		struct vector2_hasher
		{
			[[nodiscard]] auto operator()(const sf::Vector2u grid_position) const noexcept -> std::size_t
			{
				constexpr std::size_t prime = 0x9e3779b9;
				return grid_position.x ^ (grid_position.y * prime);
			}
		};

	public:
		// 当前生命值
		float health;
		// 当前魔法值(用于释放技能)
		float mana;

		EntityType selected_tower_type;

		std::unordered_map<sf::Vector2u, entt::entity, vector2_hasher> towers;
	};
}
