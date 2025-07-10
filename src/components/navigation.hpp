#pragma once

#include <vector>
#include <memory>

#include <map/navigation.hpp>

namespace components
{
	class NavigationData
	{
	public:
		using path_type = std::vector<sf::Vector2u>;

		std::unique_ptr<map::Navigation> navigation;

		// start_gate => path
		std::vector<path_type> cache_paths;
	};

	class NavigationAgent
	{
	public:
		map::Navigation::AgentId agent_id;
	};
}
