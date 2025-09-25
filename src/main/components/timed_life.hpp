#pragma once

#include <SFML/System/Time.hpp>

namespace components
{
	class TimedLife
	{
	public:
		sf::Time total;
		sf::Time elapsed;
	};
}
