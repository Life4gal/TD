#pragma once

#include <array>
#include <span>
#include <vector>
#include <optional>
#include <ranges>
#include <numbers>

#include <SFML/System/Vector2.hpp>

namespace map
{
	class TileMap;

	// ==================
	// DIRECTION
	// ==================

	enum class Direction : std::uint8_t
	{
		NONE = 0,

		NORTH,
		NORTHEAST,
		EAST,
		SOUTHEAST,
		SOUTH,
		SOUTHWEST,
		WEST,
		NORTHWEST,
	};

	constexpr std::array directions
	{
			Direction::NONE,

			Direction::NORTH,
			Direction::NORTHEAST,
			Direction::EAST,
			Direction::SOUTHEAST,
			Direction::SOUTH,
			Direction::SOUTHWEST,
			Direction::WEST,
			Direction::NORTHWEST,
	};

	constexpr auto valid_directions = directions | std::views::drop(1);

	// ==================
	// DIRECTION VALUES
	// ==================

	// 正交方向
	constexpr auto direction_cardinal_length = 1.f;
	constexpr auto direction_cardinal_normalized_length = 1.f;

	constexpr auto directional_cardinal_length_int = static_cast<int>(direction_cardinal_length);

	// 对角方向
	constexpr auto direction_diagonal_length = std::numbers::sqrt2_v<float>;
	constexpr auto direction_diagonal_normalized_length = 1.f / std::numbers::sqrt2_v<float>;

	constexpr std::array<sf::Vector2i, 9> direction_values
	{{
			// NONE
			{0, 0},
			// NORTH
			{0, -1},
			// NORTHEAST
			{1, -1},
			// EAST
			{1, 0},
			// SOUTHEAST
			{1, 1},
			// SOUTH
			{0, 1},
			// SOUTHWEST
			{-1, 1},
			// WEST
			{-1, 0},
			// NORTHWEST
			{-1, -1},
	}};

	constexpr auto valid_direction_values = direction_values | std::views::drop(1);

	constexpr std::array<sf::Vector2f, 9> direction_normalized_values
	{{
			// NONE
			{0, 0},
			// NORTH
			{0, -direction_cardinal_normalized_length},
			// NORTHEAST
			{direction_diagonal_normalized_length, -direction_diagonal_normalized_length},
			// EAST
			{direction_cardinal_normalized_length, 0},
			// SOUTHEAST
			{direction_diagonal_normalized_length, direction_diagonal_normalized_length},
			// SOUTH
			{0, direction_cardinal_normalized_length},
			// SOUTHWEST
			{-direction_diagonal_normalized_length, direction_diagonal_normalized_length},
			// WEST
			{-direction_cardinal_normalized_length, 0},
			// NORTHWEST
			{-direction_diagonal_normalized_length, -direction_diagonal_normalized_length},
	}};

	constexpr auto valid_direction_normalized_values = direction_normalized_values | std::views::drop(1);

	// ==================
	// DIRECTION + VALUES
	// ==================

	constexpr auto direction_with_values = std::views::zip(directions, direction_values);

	constexpr auto valid_direction_with_values = std::views::zip(valid_directions, valid_direction_values);

	constexpr auto direction_with_normalized_values = std::views::zip(directions, direction_normalized_values);

	constexpr auto valid_direction_with_normalized_values = std::views::zip(valid_directions, valid_direction_normalized_values);

	// ==================
	// DIRECTION ==> VALUES
	// ==================

	[[nodiscard]] constexpr auto value_of(const Direction direction) noexcept -> sf::Vector2i
	{
		return direction_values[std::to_underlying(direction)];
	}

	[[nodiscard]] constexpr auto normalized_value_of(const Direction direction) noexcept -> sf::Vector2f
	{
		return direction_normalized_values[std::to_underlying(direction)];
	}

	// ==================
	// DIRECTION ==> LENGTH
	// ==================

	[[nodiscard]] constexpr auto length_of(const Direction direction) noexcept -> float
	{
		const auto index = std::to_underlying(direction);

		// 2/4/6/8
		return (index % 2 == 0) ? direction_diagonal_length : direction_cardinal_length;
	}

	[[nodiscard]] constexpr auto normalized_length_of(const Direction direction) noexcept -> float
	{
		const auto index = std::to_underlying(direction);

		// 2/4/6/8
		return (index % 2 == 0) ? direction_diagonal_normalized_length : direction_cardinal_normalized_length;
	}

	// ==================
	// DIRECTION ==> DIRECTION
	// ==================

	[[nodiscard]] constexpr auto operator-(const Direction direction) noexcept -> Direction
	{
		// 0 --> 0
		// 1 --> 5
		// 2 --> 6
		// 3 --> 7
		// 4 --> 8
		// 5 --> 1
		// 6 --> 2
		// 7 --> 3
		// 8 --> 4
		if (direction == Direction::NONE)
		{
			return Direction::NONE;
		}

		const auto value = std::to_underlying(direction);
		return static_cast<Direction>(((value + 3) % 8) + 1);
	}

	// ==================
	// HEURISTIC
	// ==================

	class Heuristic
	{
	public:
		// 曼哈顿距离
		static auto manhattan_distance(sf::Vector2u a, sf::Vector2u b) noexcept -> float;

		// 对角距离
		static auto diagonal_distance(sf::Vector2u a, sf::Vector2u b) noexcept -> float;
	};

	using heuristic_type = auto(*)(sf::Vector2u a, sf::Vector2u b) noexcept -> float;

	// ==================
	// PATH
	// ==================

	using path_type = std::vector<sf::Vector2u>;

	class PathFinder
	{
	public:
		[[nodiscard]] static auto astar(
			const TileMap& map,
			sf::Vector2u start_point,
			sf::Vector2u end_point,
			heuristic_type heuristic = Heuristic::diagonal_distance
		) noexcept -> std::optional<path_type>;

		[[nodiscard]] static auto astar(
			const TileMap& map,
			sf::Vector2u start_point,
			std::span<const sf::Vector2u> end_points,
			heuristic_type heuristic = Heuristic::diagonal_distance
		) noexcept -> std::optional<path_type>;

		// BFS
		[[nodiscard]] static auto is_reachable(
			const TileMap& map,
			sf::Vector2u start_point,
			sf::Vector2u end_point
		) noexcept -> bool;

		// BFS
		[[nodiscard]] static auto is_reachable(
			const TileMap& map,
			sf::Vector2u start_point,
			std::span<const sf::Vector2u> end_points
		) noexcept -> bool;
	};
}
