#pragma once

#include <string>
#include <vector>

#include <map/tile_map.hpp>

#include <SFML/System/Vector2.hpp>

namespace game::component::map_ex
{
	class Info
	{
	public:
		std::string path;
	};

	class TileMap
	{
	public:
		map::TileMap map;
	};

	class Gate
	{
	public:
		std::vector<sf::Vector2u> start_gates;
		std::vector<sf::Vector2u> end_gates;
	};
}
