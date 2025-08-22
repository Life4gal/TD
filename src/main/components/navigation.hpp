#pragma once

#include <vector>

#include <map/flow_field.hpp>

#include <SFML/Graphics/VertexArray.hpp>

namespace components::navigation
{
	// 地面洋流图
	class FlowField
	{
	public:
		map::FlowField flow_field;
	};

	// 地面缓存路径
	class Path
	{
	public:
		// start_gate => path
		std::vector<map::path_type> cache_paths;
	};

	// 显示地面缓存路径
	class DebugRenderPath
	{
	public:
		sf::VertexArray paths;
	};

	// 显示地面洋流方向
	class DebugRenderFlow
	{
	public:
		sf::VertexArray directions;
	};
}
