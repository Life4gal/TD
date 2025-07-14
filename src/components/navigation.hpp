#pragma once

#include <vector>

#include <map/flow_field.hpp>

namespace components
{
	class NavigationData
	{
	public:
		map::FlowField flow_field;

		// start_gate => path
		std::vector<map::path_type> cache_paths;
	};
}
