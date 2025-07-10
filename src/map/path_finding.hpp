#pragma once

#include <span>
#include <vector>
#include <optional>

#include <SFML/System/Vector2.hpp>

namespace map
{
	class TileMap;

	class PathFinder
	{
	public:
		[[nodiscard]] static auto astar(const TileMap& map, sf::Vector2u start_point, sf::Vector2u end_point) noexcept -> std::optional<std::vector<sf::Vector2u>>;

		[[nodiscard]] static auto astar(const TileMap& map, sf::Vector2u start_point, std::span<const sf::Vector2u> end_points) noexcept -> std::optional<std::vector<sf::Vector2u>>;
	};
}
