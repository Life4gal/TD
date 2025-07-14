#pragma once

#include <vector>
#include <functional>

#include <utility/matrix.hpp>

#include <map/path.hpp>

#include <SFML/System/Vector2.hpp>

namespace map
{
	class TileMap;

	class FlowField
	{
	public:
		constexpr static auto infinity_cost = std::numeric_limits<float>::max();

	private:
		std::reference_wrapper<const TileMap> map_;
		std::vector<sf::Vector2u> end_points_;

		utility::Matrix<Direction> directions_;
		utility::Matrix<float> costs_;

	public:
		explicit FlowField(const TileMap& map) noexcept;

		auto build(std::span<const sf::Vector2u> end_points) noexcept -> void;

		auto update(sf::Vector2u point) noexcept -> void;

		[[nodiscard]] auto direction_of(sf::Vector2u point) const noexcept -> Direction;

		[[nodiscard]] auto cost_of(sf::Vector2u point) const noexcept -> float;

		[[nodiscard]] auto path_of(sf::Vector2u start_point, std::size_t max_steps = 1000) const noexcept -> std::optional<path_type>;
	};
}
