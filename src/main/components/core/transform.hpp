#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>

namespace components::transform
{
	class Position
	{
	public:
		sf::Vector2f position;
	};

	class Scale
	{
	public:
		sf::Vector2f scale;
	};

	class Rotation
	{
	public:
		sf::Angle rotation;
	};
}
