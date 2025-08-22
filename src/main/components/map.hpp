#pragma once

#include <vector>

#include <map/tile_map.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace components::map_ex
{
	class TileMap
	{
	public:
		map::TileMap tile_map;
	};

	class Gate
	{
	public:
		std::vector<sf::Vector2u> start_gates;
		std::vector<sf::Vector2u> end_gates;
	};

	class Render
	{
	public:
		// 地图图片
		sf::Sprite sprite;
	};
}
