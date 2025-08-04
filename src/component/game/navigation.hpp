#pragma once

#include <vector>

#include <map/flow_field.hpp>

namespace game::component::navigation
{
	class FlowField
	{
	public:
		::map::FlowField flow_field;
	};

	class Path
	{
	public:
		// start_gate => path
		std::vector<::map::path_type> cache_paths;
	};
} 
