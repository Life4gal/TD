#include <helper/map.hpp>

#include <components/map.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Map::load(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		using size_type = map::TileMap::size_type;

		std::vector start_gates{sf::Vector2u{7, 10}, sf::Vector2u{28, 8}};
		std::vector end_gates{sf::Vector2u{7, 21}, sf::Vector2u{30, 21}};

		// TileMap
		{
			constexpr size_type tile_width = 32;
			constexpr size_type tile_height = 32;
			constexpr size_type horizontal_tile_count = 40;
			constexpr size_type vertical_tile_count = 30;
			constexpr std::array obstacles
			{
					// 
					sf::Vector2u{5, 4},
					sf::Vector2u{6, 4},
					sf::Vector2u{7, 4},
					sf::Vector2u{8, 4},
					sf::Vector2u{9, 4},
					sf::Vector2u{10, 4},
					sf::Vector2u{11, 4},
					sf::Vector2u{12, 4},
					sf::Vector2u{13, 4},
					sf::Vector2u{14, 4},
					sf::Vector2u{15, 4},
					//
					sf::Vector2u{5, 11},
					sf::Vector2u{5, 12},
					sf::Vector2u{5, 13},
					sf::Vector2u{5, 14},
					sf::Vector2u{5, 15},
					sf::Vector2u{6, 14},
					sf::Vector2u{6, 15},
					sf::Vector2u{7, 15},
					sf::Vector2u{8, 15},
					sf::Vector2u{9, 15},
					//
					sf::Vector2u{19, 10},
					sf::Vector2u{19, 11},
					sf::Vector2u{19, 12},
					sf::Vector2u{19, 13},
					sf::Vector2u{19, 14},
					sf::Vector2u{19, 15},
					sf::Vector2u{19, 16},
					sf::Vector2u{19, 17},
					sf::Vector2u{19, 18},
					//
					sf::Vector2u{24, 11},
					sf::Vector2u{25, 11},
					sf::Vector2u{26, 11},
					sf::Vector2u{27, 11},
					sf::Vector2u{28, 11},
					sf::Vector2u{29, 11},
					sf::Vector2u{30, 11},
					sf::Vector2u{31, 11},
					//
					sf::Vector2u{35, 4},
					sf::Vector2u{35, 5},
					sf::Vector2u{35, 6},
					sf::Vector2u{35, 7},
					sf::Vector2u{35, 8},
					sf::Vector2u{35, 9},
					sf::Vector2u{35, 10},
					sf::Vector2u{35, 11},
					sf::Vector2u{35, 12},
					sf::Vector2u{35, 13},
					sf::Vector2u{35, 14},
					//
					sf::Vector2u{4, 25},
					sf::Vector2u{5, 25},
					sf::Vector2u{6, 25},
					sf::Vector2u{7, 25},
					sf::Vector2u{8, 25},
					sf::Vector2u{9, 25},
					sf::Vector2u{10, 25},
					sf::Vector2u{11, 25},
					sf::Vector2u{12, 25},
					sf::Vector2u{13, 25},
					sf::Vector2u{14, 25},
					sf::Vector2u{15, 25},
					sf::Vector2u{16, 25},
					sf::Vector2u{17, 25},
					sf::Vector2u{18, 25},
					sf::Vector2u{19, 25},
					sf::Vector2u{20, 25},
					sf::Vector2u{21, 25},
					sf::Vector2u{22, 25},
					sf::Vector2u{23, 25},
					sf::Vector2u{24, 25},
					sf::Vector2u{25, 25},
					sf::Vector2u{26, 25},
					sf::Vector2u{27, 25},
					sf::Vector2u{28, 25},
					sf::Vector2u{29, 25},
					sf::Vector2u{30, 25},
					sf::Vector2u{31, 25},
					sf::Vector2u{32, 25},
					sf::Vector2u{33, 25},
					sf::Vector2u{34, 25},
					sf::Vector2u{35, 25},
			};

			map::TileMap tile_map{tile_width, tile_height, horizontal_tile_count, vertical_tile_count};
			{
				using enum map::TileType;

				std::ranges::fill(tile_map, BUILDABLE_FLOOR);
				std::ranges::for_each(
					obstacles,
					[&](const auto point) noexcept -> void
					{
						tile_map.set(point.x, point.y, OBSTACLE);
					}
				);

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
		}

		// Gate
		{
			registry.ctx().emplace<map_ex::Gate>(std::move(start_gates), std::move(end_gates));
		}
	}
}
