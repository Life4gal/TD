#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

namespace components::limited_life
{
	// 有限存在时间
	class Time
	{
	public:
		// 剩余时间
		sf::Time remaining;
	};

	// 有限移动距离
	class Distance
	{
	public:
		// 初始位置
		sf::Vector2f initial_position;
		// 最大距离(基于entity::Position,使用平方以避免多次开方)
		float max_distance_2;
	};
}
