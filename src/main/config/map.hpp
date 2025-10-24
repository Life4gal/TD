#pragma once

#include <vector>

#include <SFML/System/Vector2.hpp>

namespace config::map_ex
{
	// 地块大小
	class TileSize
	{
	public:
		using size_type = std::uint32_t;

		size_type width;
		size_type height;
	};

	// 地块数量
	class TileCount
	{
	public:
		using size_type = TileSize::size_type;

		size_type horizontal;
		size_type vertical;
	};

	// 出生点坐标
	class StartGate
	{
	public:
		std::vector<sf::Vector2u> points;
	};

	// 终点坐标
	class EndGate
	{
	public:
		std::vector<sf::Vector2u> points;
	};

	// 普通地面坐标(可以通过,可建造)
	class BuildableFloor
	{
	public:
		std::vector<sf::Vector2u> points;
	};

	// 障碍物坐标(不可通过,不可建造)
	class Obstacle
	{
	public:
		std::vector<sf::Vector2u> points;
	};

	// 障碍物坐标(不可通过,可建造)
	class BuildableObstacle
	{
	public:
		std::vector<sf::Vector2u> points;
	};

	class Map
	{
	public:
		TileSize tile_size;
		TileCount tile_count;
		StartGate start_gate;
		EndGate end_gate;
		BuildableFloor buildable_floor;
		Obstacle obstacle;
		BuildableObstacle buildable_obstacle;
	};
}
