#pragma once

#include <vector>
#include <unordered_map>

#include <utility/hash.hpp>

#include <entt/entity/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace components::observer
{
	class GroundEnemy
	{
	public:
		// 网格 => 敌人
		std::unordered_map<sf::Vector2u, std::vector<entt::entity>, utility::vector2_hasher> entities;
		// 总数(存活)
		std::size_t total;
	};

	class AerialEnemy
	{
	public:
		// 网格 => 敌人
		std::unordered_map<sf::Vector2u, std::vector<entt::entity>, utility::vector2_hasher> entities;
		// 总数(存活)
		std::size_t total;
	};
}
