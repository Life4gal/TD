#include <map/path.hpp>

#include <algorithm>
#include <queue>
#include <ranges>
#include <functional>

#include <map/tile_map.hpp>

namespace
{
	using namespace map;

	constexpr auto unreachable_cost = std::numeric_limits<float>::max();
	constexpr auto unreachable_point = sf::Vector2u{std::numeric_limits<std::uint32_t>::max(), std::numeric_limits<std::uint32_t>::max()};

	struct node_type
	{
		// f(n) = g(n) + h(n)
		float priority;
		// g(n)
		float cost;
		// 当前节点位置
		sf::Vector2u position;

		[[nodiscard]] constexpr auto operator<(const node_type& other) const noexcept -> bool
		{
			return priority > other.priority;
		}
	};

	template<typename OnEnd, typename OnHeuristic>
	[[nodiscard]] auto do_astar(
		const TileMap& map,
		const sf::Vector2u start_point,
		const OnEnd on_end,
		const OnHeuristic on_heuristic
	) noexcept -> std::optional<path_type> //
		requires requires
		{
			{ on_end(sf::Vector2u{}) } -> std::same_as<bool>;
			{ on_heuristic(sf::Vector2u{}) } -> std::same_as<float>;
		}
	{
		const auto map_width = map.horizontal_tile_count();
		const auto map_height = map.vertical_tile_count();
		const auto to_index = [map_width](const sf::Vector2u position) noexcept -> std::uint32_t
		{
			return position.y * map_width + position.x;
		};

		std::priority_queue<node_type> open{};
		std::vector best_cost(static_cast<std::size_t>(map_width) * map_height, unreachable_cost);
		std::vector parent{static_cast<std::size_t>(map_width) * map_height, unreachable_point};

		// 将起点加入开区间
		open.emplace(on_heuristic(start_point), .0f, start_point);
		best_cost[to_index(start_point)] = .0f;
		parent[to_index(start_point)] = start_point;

		while (not open.empty())
		{
			const auto current = open.top();
			open.pop();

			// 找到终点
			if (on_end(current.position))
			{
				// 回溯构建路径
				path_type path{};
				for (auto position = current.position; position != start_point; position = parent[to_index(position)])
				{
					path.push_back(position);
				}
				path.emplace_back(start_point);

				std::ranges::reverse(path);
				return path;
			}

			// 当前路径不是最优路径则跳过
			if (current.cost > best_cost[to_index(current.position)])
			{
				continue;
			}

			// 八方向移动检查
			for (const auto [direction, direction_value]: valid_direction_with_values)
			{
				const auto next_signed = sf::Vector2i{current.position} + direction_value;

				// 下一个位置不在地图内或者无法通过
				if (not map.inside(next_signed.x, next_signed.y) or not map.passable(next_signed.x, next_signed.y))
				{
					continue;
				}

				const auto move_cost = length_of(direction) * 1.f;

				// 计算新位置的代价和优先级
				const auto next_unsigned = sf::Vector2u{next_signed};
				const auto next_cost = current.cost + move_cost;

				if (const auto index = to_index(next_unsigned);
					next_cost < best_cost[index])
				{
					best_cost[index] = next_cost;
					parent[index] = current.position;

					open.emplace(next_cost + on_heuristic(next_unsigned), next_cost, next_unsigned);
				}
			}
		}

		// 没有找到任何路径
		return std::nullopt;
	}
}

namespace map
{
	auto Heuristic::manhattan_distance(const sf::Vector2u a, const sf::Vector2u b) noexcept -> float
	{
		const auto dx = std::abs(static_cast<int>(a.x) - static_cast<int>(b.x));
		const auto dy = std::abs(static_cast<int>(a.y) - static_cast<int>(b.y));

		return static_cast<float>(dx + dy);
	}

	auto Heuristic::diagonal_distance(const sf::Vector2u a, const sf::Vector2u b) noexcept -> float
	{
		const auto dx = std::abs(static_cast<int>(a.x) - static_cast<int>(b.x));
		const auto dy = std::abs(static_cast<int>(a.y) - static_cast<int>(b.y));
		const auto min_xy = std::ranges::min(dx, dy);
		const auto max_xy = std::ranges::max(dx, dy);

		return static_cast<float>(max_xy) + (std::numbers::sqrt2_v<float> - 1.f) * static_cast<float>(min_xy);
	}

	auto PathFinder::astar(
		const TileMap& map,
		const sf::Vector2u start_point,
		const sf::Vector2u end_point,
		const heuristic_type heuristic
	) noexcept -> std::optional<path_type>
	{
		// 起点或者终点不在地图内
		if (not map.inside(start_point.x, start_point.y) or not map.inside(end_point.x, end_point.y))
		{
			return std::nullopt;
		}

		return do_astar(
			map,
			start_point,
			[end_point](const sf::Vector2u current_point) noexcept -> bool
			{
				return current_point == end_point;
			},
			[heuristic, end_point](const sf::Vector2u current_point) noexcept -> float
			{
				return heuristic(current_point, end_point);
			}
		);
	}

	auto PathFinder::astar(
		const TileMap& map,
		const sf::Vector2u start_point,
		const std::span<const sf::Vector2u> end_points,
		const heuristic_type heuristic
	) noexcept -> std::optional<path_type>
	{
		if (end_points.empty())
		{
			return std::nullopt;
		}

		if (end_points.size() == 1)
		{
			return astar(map, start_point, end_points.front(), heuristic);
		}

		return do_astar(
			map,
			start_point,
			[end_points](const sf::Vector2u current_point) noexcept -> bool
			{
				return std::ranges::contains(end_points, current_point);
			},
			[heuristic, end_points](const sf::Vector2u current_point) noexcept -> float
			{
				const auto cost_view = end_points | std::views::transform(std::bind_front(heuristic, current_point));

				return std::ranges::min(cost_view);
			}
		);
	}
}
