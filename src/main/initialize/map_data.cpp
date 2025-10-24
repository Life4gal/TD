#include <initialize/map_data.hpp>

#include <algorithm>

#include <config/map.hpp>

#include <components/map.hpp>

#include <entt/entt.hpp>

namespace
{
	[[nodiscard]] auto load_map_data() noexcept -> config::map_ex::Map
	{
		using namespace config::map_ex;

		const Map map
		{
				.tile_size = {.width = 32, .height = 32},
				.tile_count = {.horizontal = 40, .vertical = 30},
				.start_gate =
				{
						.points =
						{
								{7, 10},
								{28, 8},
						},
				},
				.end_gate =
				{
						.points =
						{
								{7, 21},
								{30, 21},
						},
				},
				.buildable_floor =
				{
						.points =
						{
								//
						},
				},
				.obstacle =
				{
						.points =
						{
								//
								{5, 4},
								{6, 4},
								{7, 4},
								{8, 4},
								{9, 4},
								{10, 4},
								{11, 4},
								{12, 4},
								{13, 4},
								{14, 4},
								{15, 4},
								//
								{5, 11},
								{5, 12},
								{5, 13},
								{5, 14},
								{5, 15},
								{6, 14},
								{6, 15},
								{7, 15},
								{8, 15},
								{9, 15},
								//
								{19, 10},
								{19, 11},
								{19, 12},
								{19, 13},
								{19, 14},
								{19, 15},
								{19, 16},
								{19, 17},
								{19, 18},
								//
								{24, 11},
								{25, 11},
								{26, 11},
								{27, 11},
								{28, 11},
								{29, 11},
								{30, 11},
								{31, 11},
								//
								{35, 4},
								{35, 5},
								{35, 6},
								{35, 7},
								{35, 8},
								{35, 9},
								{35, 10},
								{35, 11},
								{35, 12},
								{35, 13},
								{35, 14},
								//
								{4, 25},
								{5, 25},
								{6, 25},
								{7, 25},
								{8, 25},
								{9, 25},
								{10, 25},
								{11, 25},
								{12, 25},
								{13, 25},
								{14, 25},
								{15, 25},
								{16, 25},
								{17, 25},
								{18, 25},
								{19, 25},
								{20, 25},
								{21, 25},
								{22, 25},
								{23, 25},
								{24, 25},
								{25, 25},
								{26, 25},
								{27, 25},
								{28, 25},
								{29, 25},
								{30, 25},
								{31, 25},
								{32, 25},
								{33, 25},
								{34, 25},
								{35, 25},
						},
				},
				.buildable_obstacle =
				{
						.points =
						{
								//
						},
				},
		};

		return map;
	}
}

namespace initialize
{
	auto map_data(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto config = load_map_data();

		map::TileMap tile_map{config.tile_size.width, config.tile_size.height, config.tile_count.horizontal, config.tile_count.vertical};
		{
			using enum map::TileType;

			// todo: TileMap所有地块应该初始化为FLOOR,现在暂时初始化为BUILDABLE_FLOOR
			std::ranges::fill(tile_map, BUILDABLE_FLOOR);

			std::ranges::for_each(
				config.buildable_floor.points,
				[&](const sf::Vector2u point) noexcept -> void
				{
					tile_map.set(point.x, point.y, BUILDABLE_FLOOR);
				}
			);
			std::ranges::for_each(
				config.obstacle.points,
				[&](const sf::Vector2u point) noexcept -> void
				{
					tile_map.set(point.x, point.y, OBSTACLE);
				}
			);
			std::ranges::for_each(
				config.buildable_obstacle.points,
				[&](const sf::Vector2u point) noexcept -> void
				{
					tile_map.set(point.x, point.y, BUILDABLE_OBSTACLE);
				}
			);

			// 确保起点和终点是不可建造地板
			std::ranges::for_each(
				config.start_gate.points,
				[&](const sf::Vector2u point) noexcept -> void
				{
					tile_map.set(point.x, point.y, FLOOR);
				}
			);
			std::ranges::for_each(
				config.end_gate.points,
				[&](const sf::Vector2u point) noexcept -> void
				{
					tile_map.set(point.x, point.y, FLOOR);
				}
			);
		}

		registry.ctx().emplace<map_ex::TileMap>(std::move(tile_map));
		registry.ctx().emplace<map_ex::StartGate>(std::move(config.start_gate));
		registry.ctx().emplace<map_ex::EndGate>(std::move(config.end_gate));
	}
}
