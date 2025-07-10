#include <map/path_finding.hpp>

#include <algorithm>
#include <array>
#include <queue>
#include <ranges>

#include <map/tile_map.hpp>

namespace
{
	constexpr auto unreachable_cost = std::numeric_limits<std::uint32_t>::max();
	constexpr auto unreachable_point = sf::Vector2u{std::numeric_limits<std::uint32_t>::max(), std::numeric_limits<std::uint32_t>::max()};

	constexpr std::array<sf::Vector2i, 4> directions{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

	struct node_type
	{
		// f(n) = g(n) + h(n)
		std::uint32_t priority;
		// g(n)
		std::uint32_t cost;
		// 当前节点位置
		sf::Vector2u position;

		[[nodiscard]] constexpr auto operator<(const node_type& other) const noexcept -> bool
		{
			return priority > other.priority;
		}
	};

	// 曼哈顿距离启发式
	[[nodiscard]] auto heuristic_manhattan_distance(const sf::Vector2u a, const sf::Vector2u b) noexcept -> std::uint32_t
	{
		return std::abs(static_cast<int>(a.x) - static_cast<int>(b.x)) + std::abs(static_cast<int>(a.y) - static_cast<int>(b.y));
	}
}

namespace map
{
	auto PathFinder::astar(const TileMap& map, const sf::Vector2u start_point, const sf::Vector2u end_point) noexcept -> std::optional<std::vector<sf::Vector2u>>
	{
		// 起点或者终点不在地图内
		if (not map.inside(start_point.x, start_point.y) or not map.inside(end_point.x, end_point.y))
		{
			return std::nullopt;
		}

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
		open.emplace(heuristic_manhattan_distance(start_point, end_point), 0, start_point);
		best_cost[to_index(start_point)] = 0;
		parent[to_index(start_point)] = start_point;

		while (not open.empty())
		{
			const auto current = open.top();
			open.pop();

			// 找到终点
			if (current.position == end_point)
			{
				// 回溯构建路径
				std::vector<sf::Vector2u> path{};
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

			// 四方向移动检查
			for (const auto direction: directions)
			{
				const auto next_signed = sf::Vector2i{current.position} + direction;

				// 下一个位置不在地图内或者无法通过
				if (not map.inside(next_signed.x, next_signed.y) or not map.passable(next_signed.x, next_signed.y))
				{
					continue;
				}

				// 计算新位置的代价和优先级
				const auto next_unsigned = sf::Vector2u{next_signed};
				const auto next_cost = current.cost + 1;

				if (const auto index = to_index(next_unsigned);
					next_cost < best_cost[index])
				{
					best_cost[index] = next_cost;
					parent[index] = current.position;

					open.emplace(next_cost + heuristic_manhattan_distance(next_unsigned, end_point), next_cost, next_unsigned);
				}
			}
		}

		// 没有找到任何路径
		return std::nullopt;
	}

	auto PathFinder::astar(const TileMap& map, const sf::Vector2u start_point, const std::span<const sf::Vector2u> end_points) noexcept -> std::optional<std::vector<sf::Vector2u>>
	{
		if (end_points.empty())
		{
			return std::nullopt;
		}

		if (end_points.size() == 1)
		{
			return astar(map, start_point, end_points.front());
		}

		auto best = astar(map, start_point, end_points.front());
		for (const auto end_point: end_points | std::views::drop(1))
		{
			auto new_path = astar(map, start_point, end_point);

			if (not new_path.has_value())
			{
				continue;
			}

			if (not best.has_value())
			{
				best = new_path;
				continue;
			}

			if (new_path->size() < best->size())
			{
				best.swap(new_path);
			}
		}

		return best;
	}
}
