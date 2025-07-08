#pragma once

#include <vector>

#include <map/tile_map.hpp>

#include <SFML/System/Vector2.hpp>

namespace components
{
	class MapData
	{
	public:
		map::TileMap map;

		std::vector<sf::Vector2u> start_gates;
		std::vector<sf::Vector2u> end_gates;

		std::uint32_t enemy_counter;
		std::uint32_t enemy_kill_counter;
		std::uint32_t tower_counter;
	};
}
