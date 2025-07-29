#pragma once

#include <vector>

#include <map/tile_map.hpp>

#include <SFML/System/Vector2.hpp>

namespace components::map_ex
{
	class Map
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

	class Counter
	{
	public:
		using size_type = std::uint32_t;

		size_type alive_enemy;
		size_type killed_enemy;
		size_type built_tower;
	};
}
