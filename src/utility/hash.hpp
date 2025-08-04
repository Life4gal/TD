#pragma once

#include <SFML/System/Vector2.hpp>

namespace utility
{
	struct vector2_hasher
	{
		[[nodiscard]] auto operator()(const sf::Vector2u grid_position) const noexcept -> std::size_t
		{
			constexpr std::size_t prime = 0x9e3779b9;
			return grid_position.x ^ (grid_position.y * prime);
		}
	};
} 
