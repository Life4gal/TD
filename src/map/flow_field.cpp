#include <map/flow_field.hpp>

#include <algorithm>
#include <queue>
#include <ranges>

#include <map/tile_map.hpp>

namespace
{
	using namespace map;

	struct node_type
	{
		float cost;
		sf::Vector2u position;

		[[nodiscard]] constexpr auto operator<(const node_type& other) const noexcept -> bool
		{
			return cost > other.cost;
		}
	};
}

namespace map
{
	FlowField::FlowField(const TileMap& map) noexcept
		: map_{map},
		  directions_{map.horizontal_tile_count(), map.vertical_tile_count(), Direction::NONE},
		  costs_{map.horizontal_tile_count(), map.vertical_tile_count(), infinity_cost}
	{
		//
	}

	auto FlowField::build(const std::span<const sf::Vector2u> end_points) noexcept -> void
	{
		const auto& map = map_.get();

		end_points_.clear();
		// 仅保留有效终点
		std::ranges::copy_if(
			end_points,
			std::back_inserter(end_points_),
			[&map](const auto point) noexcept -> bool
			{
				return map.inside(point.x, point.y) and map.passable(point.x, point.y);
			}
		);
		assert(not end_points_.empty());
		// 重置状态
		std::ranges::fill(directions_, Direction::NONE);
		std::ranges::fill(costs_, infinity_cost);

		std::priority_queue<node_type> queue{};
		for (const auto point: end_points_)
		{
			auto& direction = directions_[point.x, point.y];
			auto& cost = costs_[point.x, point.y];

			direction = Direction::NONE;
			cost = .0f;

			queue.emplace(cost, point);
		}

		while (not queue.empty())
		{
			const auto [current_cost, current_position] = queue.top();
			queue.pop();

			// 当前路径不是最优则跳过
			if (current_cost > costs_[current_position.x, current_position.y])
			{
				continue;
			}

			// 检查所有相邻节点
			for (const auto [direction, direction_value]: valid_direction_with_values)
			{
				const auto next_signed = sf::Vector2i{current_position} + direction_value;

				// 下一个节点不在地图内或者无法通过
				if (not map.inside(next_signed.x, next_signed.y) or not map.passable(next_signed.x, next_signed.y))
				{
					continue;
				}

				const auto move_cost = map::length_of(direction) * 1.f;

				const auto next_unsigned = sf::Vector2u{next_signed};
				auto& next_direction = directions_[next_unsigned.x, next_unsigned.y];
				auto& next_cost = costs_[next_unsigned.x, next_unsigned.y];

				if (const auto new_cost = current_cost + move_cost;
					new_cost < next_cost)
				{
					next_direction = -direction;
					next_cost = new_cost;

					queue.emplace(new_cost, next_unsigned);
				}
			}
		}
	}

	auto FlowField::update(const sf::Vector2u point) noexcept -> void
	{
		auto& map = map_.get();

		if (not map.inside(point.x, point.y))
		{
			return;
		}

		// todo: 增量更新
		build(end_points_);
	}

	auto FlowField::direction_of(const sf::Vector2u point) const noexcept -> Direction
	{
		if (const auto& map = map_.get();
			not map.inside(point.x, point.y) or not map.passable(point.x, point.y))
		{
			return Direction::NONE;
		}

		return directions_[point.x, point.y];
	}

	auto FlowField::cost_of(const sf::Vector2u point) const noexcept -> float
	{
		if (const auto& map = map_.get();
			not map.inside(point.x, point.y) or not map.passable(point.x, point.y))
		{
			return infinity_cost;
		}

		return costs_[point.x, point.y];
	}

	auto FlowField::path_of(const sf::Vector2u start_point, const std::size_t max_steps) const noexcept -> std::optional<path_type>
	{
		if (const auto& map = map_.get();
			not map.inside(start_point.x, start_point.y) or not map.passable(start_point.x, start_point.y))
		{
			return std::nullopt;
		}

		path_type path{};
		path.emplace_back(start_point);

		// 起点即为终点
		if (directions_[start_point.x, start_point.y] == Direction::NONE)
		{
			return path;
		}

		auto current_point = sf::Vector2i{start_point};
		for (std::size_t current_step = 0; current_step < max_steps; ++current_step)
		{
			const auto current_direction = directions_[current_point.x, current_point.y];

			// 到达终点
			if (current_direction == Direction::NONE)
			{
				break;
			}

			current_point += value_of(current_direction);
			path.emplace_back(current_point);
		}

		return path;
	}
}
