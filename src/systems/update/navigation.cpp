#include <systems/update/navigation.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/enemy.hpp>
#include <components/navigation.hpp>
#include <components/map.hpp>

#include <entt/entt.hpp>

namespace systems::update
{
	auto navigation(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		const auto delta_time = delta.asSeconds();

		const auto& [map] = registry.ctx().get<const map_ex::Map>();
		const auto half_tile = sf::Vector2f{static_cast<float>(map.tile_width()) * .5f, static_cast<float>(map.tile_height()) * .5f,};

		const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();

		// todo: 导航系统依然存在问题,在进行对角移动时,依然会意外地进入非路径网格
		// 标记为到达终点
		auto mark_reached = [&registry](const entt::entity entity) noexcept -> void
		{
			registry.remove<tags::enemy_alive>(entity);
			registry.emplace<tags::enemy_reached>(entity);
		};

		for (const auto enemy_view = registry.view<tags::enemy, tags::enemy_alive, Position, const enemy::Movement>();
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
}
