#pragma once

#include <SFML/System/Vector2.hpp>

namespace components
{
	class WorldPosition
	{
	public:
		sf::Vector2f position;
	};

	class Movement
	{
	public:
		float speed;
	};

	class Health
	{
	public:
		float health;
	};
}
