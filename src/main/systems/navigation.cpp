#include <systems/navigation.hpp>

#include <algorithm>
#include <ranges>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/enemy.hpp>
#include <components/navigation.hpp>

#include <components/map.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Navigation::initialize(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();
		const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();

		map::FlowField flow_field{tile_map};
		{
			flow_field.build(end_gates);
		}

		std::vector<map::path_type> cache_paths{};
		{
			cache_paths.reserve(start_gates.size());

			// 为每个起点到最佳终点计算路径
			for (const auto start: start_gates)
			{
				auto path = flow_field.path_of(start, std::numeric_limits<std::size_t>::max());
				assert(path.has_value());

				cache_paths.emplace_back(std::move(*path));
			}
		}

		registry.ctx().emplace<navigation::FlowField>(std::move(flow_field));
		registry.ctx().emplace<navigation::Path>(std::move(cache_paths));

		// debug
		{
			registry.ctx().emplace<navigation::DebugRenderPath>(sf::VertexArray{sf::PrimitiveType::LineStrip});
			registry.ctx().emplace<navigation::DebugRenderFlow>(sf::VertexArray{sf::PrimitiveType::Lines});
		}
	}

	auto Navigation::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		const auto delta_time = delta.asSeconds();

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();
		const auto half_tile = sf::Vector2f{static_cast<float>(tile_map.tile_width()) * .5f, static_cast<float>(tile_map.tile_height()) * .5f,};

		const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();

		// 标记为到达终点
		auto mark_reached = [&registry](const entt::entity entity) noexcept -> void
		{
			registry.emplace<tags::dead>(entity);
			registry.emplace<tags::enemy_reached>(entity);
		};

		for (const auto enemy_view = registry.view<tags::archetype_aerial, entity::Position, enemy::Movement>(entt::exclude<tags::dead>);
		     const auto [entity, position, movement]: enemy_view.each())
		{
			// todo: 空中导航
			std::ignore = entity;
		}

		// 地面导航
		for (const auto enemy_view = registry.view<tags::archetype_ground, entity::Position, enemy::Movement, enemy::Direction>(entt::exclude<tags::dead>);
		     const auto [entity, position, movement, direction]: enemy_view.each())
		{
			// 本帧移动距离
			const auto total_move_distance = movement.speed * delta_time;

			// 剩余移动距离
			auto remaining_distance = total_move_distance;

			// 这两个变量挪到此作用域是因为它们仅在跨越网格变动
			// 当前网格点
			auto current_point = tile_map.coordinate_world_to_grid(position.position);
			// 当前网格中心点
			auto current_point_center_position = tile_map.coordinate_grid_to_world(current_point);

			while (remaining_distance > 0)
			{
				if (direction.direction == map::Direction::NONE)
				{
					mark_reached(entity);
					break;
				}

				// 与中心点偏移(用于计算可移动距离)
				const auto offset_from_center = position.position - current_point_center_position;
				// 方向向量
				const auto direction_value = map::value_of(direction.direction);
				const auto direction_normalized_value = map::normalized_value_of(direction.direction);

				// 网格流向
				// 敌人方向 == 网格流向:
				// 1.此网格流向与上一个网格相同
				// 2.此网格流向与上一个网格不同,且敌人经过此网格中心点
				// 敌人方向 != 网格流向
				// 1.此网格流向与上一个网格不同,且敌人未经过中心点
				if (const auto flow_direction = flow_field.direction_of(current_point);
					direction.direction == flow_direction)
				{
					// 中心->边界

					// 到边界的距离
					const auto max_movable_distance = [&]
					{
						auto distance = std::numeric_limits<float>::max();

						if (direction_value.x != 0)
						{
							const auto dx =
							(
								(direction_value.x > 0 ? half_tile.x : -half_tile.x) - offset_from_center.x
							) / direction_normalized_value.x;

							distance = std::ranges::min(distance, dx);
						}
						if (direction_value.y != 0)
						{
							const auto dy =
							(
								(direction_value.y > 0 ? half_tile.y : -half_tile.y) - offset_from_center.y
							) / direction_normalized_value.y;

							distance = std::ranges::min(distance, dy);
						}

						return distance;
					}();

					// 本段实际移动距离
					const auto current_move_distance = std::ranges::min(remaining_distance, max_movable_distance);
					const auto next_position = position.position + current_move_distance * direction_normalized_value;

					position.position = next_position;
					remaining_distance -= current_move_distance;

					// 如果移动距离超过到边界距离
					if (current_move_distance >= max_movable_distance)
					{
						// 进入下一个网格
						const auto next_point_signed = sf::Vector2i{current_point} + direction_value;
						const auto next_point_unsigned = sf::Vector2u{next_point_signed};

						current_point = next_point_unsigned;
						current_point_center_position = tile_map.coordinate_grid_to_world(current_point);
					}
					else
					{
						// 结束
						break;
					}
				}
				else
				{
					// 边界->中心

					// 到中心点的距离
					const auto max_movable_distance = [&]
					{
						auto distance = std::numeric_limits<float>::max();

						if (direction_value.x != 0)
						{
							const auto dx = -offset_from_center.x / direction_normalized_value.x;

							distance = std::ranges::min(distance, dx);
						}
						if (direction_value.y != 0)
						{
							const auto dy = -offset_from_center.y / direction_normalized_value.y;

							distance = std::ranges::min(distance, dy);
						}

						return distance;
					}();
					// 本段实际移动距离
					const auto current_move_distance = std::ranges::min(remaining_distance, max_movable_distance);

					position.position += current_move_distance * direction_normalized_value;
					remaining_distance -= current_move_distance;

					// 如果移动距离超过到中心点距离
					if (current_move_distance >= max_movable_distance)
					{
						// 改变移动方向(按照网格流向移动)
						direction.direction = flow_direction;
					}
				}
			}
		}
	}

	auto Navigation::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();
		const auto& [cache_paths] = registry.ctx().get<const navigation::Path>();

		// 缓存路径
		if (auto* render = registry.ctx().find<navigation::DebugRenderPath>())
		{
			auto& paths = render->paths;

			std::ranges::for_each(
				cache_paths,
				[&](const auto& path) noexcept -> void
				{
					std::ranges::for_each(
						path,
						[&](const auto point) noexcept -> void
						{
							const auto world_position = tile_map.coordinate_grid_to_world(point);

							paths.append({.position = world_position, .color = sf::Color::Green, .texCoords = {}});
						}
					);

					window.draw(paths);
					paths.clear();
				}
			);
		}

		// 网格流向
		if (auto* render = registry.ctx().find<navigation::DebugRenderFlow>())
		{
			auto& directions = render->directions;

			for (std::uint32_t y = 0; y < tile_map.vertical_tile_count(); ++y)
			{
				for (std::uint32_t x = 0; x < tile_map.horizontal_tile_count(); ++x)
				{
					constexpr auto arrow_length = 15.f;
					constexpr auto arrow_head_length = arrow_length * .35f;

					const auto direction = flow_field.direction_of({x, y});
					const auto direction_value = sf::Vector2f{map::value_of(direction)};

					const auto start_position = tile_map.coordinate_grid_to_world(sf::Vector2u{x, y});
					const auto end_position = start_position + direction_value * arrow_length;
					const auto arrow_offset = direction_value * arrow_head_length;

					directions.append({.position = start_position, .color = sf::Color::Red, .texCoords = {}});
					directions.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});

					const auto perpendicular = direction_value.perpendicular();
					const auto arrow_position_1 = end_position - arrow_offset + perpendicular * arrow_head_length * .5f;
					const auto arrow_position_2 = end_position - arrow_offset - perpendicular * arrow_head_length * .5f;

					directions.append({.position = arrow_position_1, .color = sf::Color::Red, .texCoords = {}});
					directions.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});
					directions.append({.position = arrow_position_2, .color = sf::Color::Red, .texCoords = {}});
					directions.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});
				}
			}

			window.draw(directions);
			directions.clear();
		}
	}
}
