#pragma once

#include <unordered_map>

#include <utility/hash.hpp>

#include <component/game/entity.hpp>
#include <component/game/resource.hpp>

#include <SFML/System/Vector2.hpp>

namespace game::component::player
{
	class Interaction
	{
	public:
		// 当前选择的塔(用于建造塔)
		entity::Type selected_tower_type;
		// 当前选择的武器(用于给塔装备武器)
		entity::Type selected_weapon_type;
	};

	// 建造的塔
	class Tower
	{
	public:
		// 网格 => 塔
		std::unordered_map<sf::Vector2u, entt::entity, utility::vector2_hasher> tower;
	};

	// 资源
	class Resource
	{
	public:
		// 资源类型 ==> 资源数量
		std::unordered_map<resource::Type, resource::size_type> resource;
	};

	// 击杀数
	class Statistics
	{
	public:
		using size_type = std::uint32_t;

		size_type killed_enemy;
	};
}
