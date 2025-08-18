#include <system/game/helper/map.hpp>

#include <component/game/map.hpp>

#include <entt/entt.hpp>

namespace game::system::helper
{
	auto Map::load(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		using size_type = map::TileMap::size_type;

		// todo: 参数应该怎么传?传文件名还是地图名?
		const auto& [current_map] = registry.ctx().get<const map_ex::Info>();
		std::ignore = current_map;

		constexpr size_type tile_width = 32;
		constexpr size_type tile_height = 32;
		constexpr size_type horizontal_tile_count = 40;
		constexpr size_type vertical_tile_count = 30;

		std::vector start_gates{sf::Vector2u{1, 1}, sf::Vector2u{20, 3}};
		std::vector end_gates{sf::Vector2u{1, 20}, sf::Vector2u{10, 15}, sf::Vector2u{20, 20}, sf::Vector2u{15, 10}};

		map::TileMap tile_map{tile_width, tile_height, horizontal_tile_count, vertical_tile_count};
		{
			using enum map::TileType;

			std::ranges::fill(tile_map, BUILDABLE_FLOOR);

			// 创建一些障碍物
			tile_map.set(10, 10, OBSTACLE);
			// ...

			// 确保起点和终点是不可建造地板
			for (const auto gate: start_gates)
			{
				tile_map.set(gate.x, gate.y, FLOOR);
			}
			for (const auto gate: end_gates)
			{
				tile_map.set(gate.x, gate.y, FLOOR);
			}
		}

		registry.ctx().emplace<map_ex::TileMap>(std::move(tile_map));
		registry.ctx().emplace<map_ex::Gate>(std::move(start_gates), std::move(end_gates));
	}
}
