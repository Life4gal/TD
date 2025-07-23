#pragma once

#include <vector>

#include <entt/entity/entity.hpp>

#include <SFML/System/Vector2.hpp>

namespace components
{
	class GridPosition
	{
	public:
		sf::Vector2u position;
	};

	class Modifier
	{
	public:
		// todo: 不同的塔提供不同的增益效果
		int none;
	};

	class Equipment
	{
	public:
		// 一个塔可以有多个武器
		std::vector<entt::entity> weapons;
	};
}
