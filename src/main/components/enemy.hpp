#pragma once

#include <map/path.hpp>

#include <SFML/Graphics/CircleShape.hpp>

namespace components::enemy
{
	class Health
	{
	public:
		float health;
	};

	class Movement
	{
	public:
		float speed;
	};

	class Power
	{
	public:
		using value_type = std::uint32_t;

		value_type power;
	};

	// 只有地面单位才需要(洋流)方向
	class Direction
	{
	public:
		map::Direction direction;
	};

	class Render
	{
	public:
		sf::CircleShape shape;
	};
}
