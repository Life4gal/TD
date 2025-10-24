#pragma once

#include <unordered_map>

#include <utility/hash.hpp>

#include <components/combat/unit.hpp>
#include <components/game/resource.hpp>

#include <entt/entity/fwd.hpp>

#include <SFML/Graphics/RectangleShape.hpp>

namespace components::player
{
	// 玩家交互组件
	class Interaction
	{
	public:
		// 当前选择的塔
		combat::Type selected_tower_type;
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

	class Cursor
	{
	public:
		sf::RectangleShape cursor;
	};
}
