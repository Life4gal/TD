#pragma once

#include <unordered_map>

#include <components/entity.hpp>
#include <components/resource.hpp>

#include <SFML/System/Vector2.hpp>

namespace components::player
{
	class Interaction
	{
	public:
		// 当前选择的塔(用于建造塔)
		EntityType selected_tower_type;
		// 当前选择的武器(用于给塔装备武器)
		EntityType selected_weapon_type;
	};

	class Resource
	{
	public:
		std::unordered_map<resource::Type, resource::size_type> resources;
	};

	class Tower
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
		std::unordered_map<sf::Vector2u, entt::entity, vector2_hasher> towers;
	};
}
