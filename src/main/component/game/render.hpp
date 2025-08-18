#pragma once

#include <manager/font.hpp>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace game::component::render
{
	class Map
	{
	public:
		sf::RectangleShape tile_shape;
		sf::CircleShape gate_shape;
	};

	class Navigation
	{
	public:
		sf::VertexArray path_vertices;
	};

	class Player
	{
	public:
		sf::RectangleShape cursor_shape;
	};

	// ReSharper disable once CppInconsistentNaming
	class HUD
	{
	public:
		manager::Font::FontId hud_font;
		sf::Text hud_text;
	};

	class Tower
	{
	public:
		sf::CircleShape shape;
	};

	class Weapon
	{
	public:
		sf::VertexArray target_line;
	};

	class Enemy
	{
	public:
		sf::CircleShape shape;
	};
}
