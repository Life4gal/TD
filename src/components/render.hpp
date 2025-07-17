#pragma once

#include <manager/font.hpp>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace components
{
	class RenderMapData
	{
	public:
		sf::RectangleShape tile_shape;
		sf::CircleShape gate_shape;
		sf::RectangleShape cursor_shape;
	};

	// ReSharper disable once CppInconsistentNaming
	class RenderHUDData
	{
	public:
		manager::Font::FontId hud_font;
		sf::Text hud_text;
	};

	class RenderNavigationData
	{
	public:
		sf::VertexArray path_vertices;
	};

	class RenderEnemyData
	{
	public:
		sf::CircleShape shape;
	};
}
