#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace components::renderable
{
	class Texture
	{
	public:
		entt::id_type id;
	};

	class Area
	{
	public:
		sf::IntRect area;
	};

	class Origin
	{
	public:
		sf::Vector2f origin;
	};

	class Color
	{
	public:
		sf::Color color;
	};
}
