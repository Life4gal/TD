#pragma once

#include <map/tile_map.hpp>
#include <config/map.hpp>

#include <SFML/Graphics/Sprite.hpp>

namespace components::map_ex
{
	class TileMap
	{
	public:
		map::TileMap tile_map;
	};

	using config::map_ex::EndGate;
	using config::map_ex::StartGate;

	// todo: 把这个组件放到一个合适的位置
	class Background
	{
	public:
		// 地图图片
		sf::Sprite sprite;
	};
}
