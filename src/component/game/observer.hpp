#pragma once

#include <vector>
#include <unordered_map>

#include <utility/hash.hpp>

#include <entt/entity/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace game::component::observer
{
	class EnemyArchetype
	{
	public:
		// 网格 => 敌人

		// 地面
		std::unordered_map<sf::Vector2u, std::vector<entt::entity>, utility::vector2_hasher> ground;
		// 空中
		std::unordered_map<sf::Vector2u, std::vector<entt::entity>, utility::vector2_hasher> aerial;
	};

	class EnemyStatistics
	{
	public:
		using size_type = std::uint32_t;

		// 当前存活的敌人数量
		size_type alive;
	};
}
