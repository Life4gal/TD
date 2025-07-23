#include <systems/navigation.hpp>

#include <components/tags.hpp>
#include <components/enemy.hpp>
#include <components/render.hpp>

#include <components/map.hpp>
#include <components/navigation.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Navigation::initialize(entt::registry& registry) noexcept -> void
	{
		const auto& map_data = registry.ctx().get<components::MapData>();
		const auto& map = map_data.map;

		map::FlowField flow_field{map};
		{
			flow_field.build(map_data.end_gates);
		}

		std::vector<map::path_type> cache_paths{};
		{
			cache_paths.reserve(map_data.start_gates.size());

			for (const auto start_point: map_data.start_gates)
			{
				auto path = flow_field.path_of(start_point, std::numeric_limits<std::size_t>::max());
				assert(path.has_value());

				cache_paths.emplace_back(*std::move(path));
			}
		}

		// render
		{
			components::RenderNavigationData render_navigation_data
			{
					.path_vertices = sf::VertexArray{sf::PrimitiveType::LineStrip},
			};

			registry.ctx().emplace<components::RenderNavigationData>(std::move(render_navigation_data));
		}

		components::NavigationData navigation_data
		{
				.flow_field = std::move(flow_field),
				.cache_paths = std::move(cache_paths),
		};

		registry.ctx().emplace<components::NavigationData>(std::move(navigation_data));
	}

	auto Navigation::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		const auto delta_time = delta.asSeconds();

		const auto& map_data = registry.ctx().get<components::MapData>();
		const auto& map = map_data.map;
		const auto half_tile = sf::Vector2f{static_cast<float>(map.tile_width()) * .5f, static_cast<float>(map.tile_height()) * .5f,};

		const auto& navigation_data = registry.ctx().get<components::NavigationData>();
		const auto& flow_field = navigation_data.flow_field;

		// 标记为到达终点
		auto mark_reached = [&registry](const entt::entity entity) noexcept -> void
		{
			registry.remove<components::tags::enemy_alive>(entity);
			registry.emplace<components::tags::enemy_reached>(entity);
		};

		for (const auto enemy_view = registry.view<components::tags::enemy, components::tags::enemy_alive, components::WorldPosition, components::Movement>();
		     auto [entity, position, movement]: enemy_view.each())
		{
			// 本帧移动距离
			const auto total_move_distance = movement.speed * delta_time;

			// 剩余移动距离
			auto remaining_move_distance = total_move_distance;
			// 当前网格点
			auto current_point = map.coordinate_world_to_grid(position.position);

			while (remaining_move_distance > 0)
			{
				// 需要区分移动方向
				// 边界 => 网格中心 (向中心移动)
				// 网格中心 => 边界 (按照流场流向)

				const auto current_point_center_position = map.coordinate_grid_to_world(current_point);
				const auto offset_from_center = position.position - current_point_center_position;

				constexpr auto epsilon = 1e-4f;
				const auto sign_of_offset_from_center = sf::Vector2i
				{
						(offset_from_center.x > epsilon) - (offset_from_center.x < -epsilon),
						(offset_from_center.y > epsilon) - (offset_from_center.y < -epsilon),
				};

				const auto flow_direction = flow_field.direction_of(current_point);
				const auto flow_direction_value = map::value_of(flow_direction);

				if (const auto crossed_center =
					(
						flow_direction_value.x * sign_of_offset_from_center.x +
						flow_direction_value.y * sign_of_offset_from_center.y
					) >= 0;
					crossed_center)
				{
					// 中心 ==> 边界
					const auto current_direction = flow_direction;

					if (current_direction == map::Direction::NONE)
					{
						// 到达终点
						mark_reached(entity);
						break;
					}

					const auto direction_value = map::value_of(current_direction);
					const auto direction_normalized_value = map::normalized_value_of(current_direction);

					// 到边界距离
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

					// 此次移动实际移动距离
					const auto current_move_distance = std::ranges::min(remaining_move_distance, max_movable_distance);

					position.position += current_move_distance * direction_normalized_value;
					remaining_move_distance -= current_move_distance;

					// 如果移动距离超过到达边界距离
					if (current_move_distance >= max_movable_distance)
					{
						// 进入下一个网格
						current_point = sf::Vector2u{sf::Vector2i{current_point} + direction_value};
					}
					else
					{
						// 结束移动
						break;
					}
				}
				else
				{
					// 边界 ==> 中心

					constexpr map::Direction sign_table[3][3]
					{
							/* y = -1 */
							{map::Direction::NORTHWEST, map::Direction::NORTH, map::Direction::NORTHEAST},
							/* y = 0 */
							{map::Direction::WEST, map::Direction::NONE, map::Direction::EAST},
							/* y = 1 */
							{map::Direction::SOUTHWEST, map::Direction::SOUTH, map::Direction::SOUTHEAST},
					};

					const auto current_direction = -sign_table[sign_of_offset_from_center.y + 1][sign_of_offset_from_center.x + 1];

					if (current_direction == map::Direction::NONE)
					{
						// 到达终点
						mark_reached(entity);
						break;
					}

					// const auto direction_value = map::value_of(current_direction);
					const auto direction_normalized_value = map::normalized_value_of(current_direction);

					const auto max_movable_distance = (-offset_from_center.x * direction_normalized_value.x) + (-offset_from_center.y * direction_normalized_value.y);

					// 此次移动实际移动距离
					const auto current_move_distance = std::ranges::min(remaining_move_distance, max_movable_distance);

					position.position += current_move_distance * direction_normalized_value;
					remaining_move_distance -= current_move_distance;
				}
			}
		}
	}

	auto Navigation::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		if (auto* render_navigation_data = registry.ctx().find<components::RenderNavigationData>())
		{
			const auto& map_data = registry.ctx().get<components::MapData>();
			const auto& map = map_data.map;

			const auto& navigation_data = registry.ctx().get<components::NavigationData>();
			const auto& flow_field = navigation_data.flow_field;
			const auto& cache_paths = navigation_data.cache_paths;

			auto& path_vertices = render_navigation_data->path_vertices;

			std::ranges::for_each(
				cache_paths,
				[&](const auto& path) noexcept -> void
				{
					std::ranges::for_each(
						path,
						[&](const auto point) noexcept -> void
						{
							const auto world_position = map.coordinate_grid_to_world(point);

							path_vertices.append({.position = world_position, .color = sf::Color::Green, .texCoords = {}});
						}
					);

					window.draw(path_vertices);
					path_vertices.clear();
				}
			);

			// 绘制流场方向
			sf::VertexArray lines{sf::PrimitiveType::Lines};
			const auto arrow_length = static_cast<float>(std::ranges::min(map.tile_width(), map.tile_height())) * .5f;
			const auto arrow_head_length = arrow_length * .35f;
			for (map::TileMap::size_type y = 0; y < map.vertical_tile_count(); ++y)
			{
				for (map::TileMap::size_type x = 0; x < map.horizontal_tile_count(); ++x)
				{
					const auto direction = flow_field.direction_of({x, y});
					const auto direction_value = sf::Vector2f{map::value_of(direction)};

					const auto start_position = map.coordinate_grid_to_world(sf::Vector2u{x, y});
					const auto end_position = start_position + direction_value * arrow_length;
					const auto arrow_offset = direction_value * arrow_head_length;

					lines.append({.position = start_position, .color = sf::Color::Red, .texCoords = {}});
					lines.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});

					const auto perpendicular = direction_value.perpendicular();
					const auto arrow_position_1 = end_position - arrow_offset + perpendicular * arrow_head_length * .5f;
					const auto arrow_position_2 = end_position - arrow_offset - perpendicular * arrow_head_length * .5f;

					lines.append({.position = arrow_position_1, .color = sf::Color::Red, .texCoords = {}});
					lines.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});
					lines.append({.position = arrow_position_2, .color = sf::Color::Red, .texCoords = {}});
					lines.append({.position = end_position, .color = sf::Color::Red, .texCoords = {}});
				}
			}

			window.draw(lines);
		}
	}
}
