#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

namespace components
{
	class RenderMapData
	{
	public:
		sf::RectangleShape tile_shape;
		sf::CircleShape gate_shape;
		sf::RectangleShape cursor_shape;
	};
}
