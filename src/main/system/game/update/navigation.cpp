#include <system/game/update/navigation.hpp>

#include <component/game/tags.hpp>
#include <component/game/entity.hpp>
#include <component/game/enemy.hpp>
#include <component/game/navigation.hpp>
#include <component/game/map.hpp>

#include <entt/entt.hpp>

namespace game::system::update
{
	auto navigation(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace component;

		const auto delta_time = delta.asSeconds();

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();
		const auto half_tile = sf::Vector2f{static_cast<float>(tile_map.tile_width()) * .5f, static_cast<float>(tile_map.tile_height()) * .5f,};

		const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();

		// 标记为到达终点
		auto mark_reached = [&registry](const entt::entity entity) noexcept -> void
		{
			registry.remove<tags::enemy::status::alive>(entity);
			registry.emplace<tags::enemy::status::reached>(entity);
		};

		// todo: 空中导航

		// 地面导航
		for (const auto enemy_view = registry.view<tags::enemy::identifier, tags::enemy::status::alive, tags::enemy::archetype::ground, entity::Position, enemy::Direction, const enemy::Movement>();
		     auto [entity, position, direction, movement]: enemy_view.each())
		{
			// 本帧移动距离
			const auto total_move_distance = movement.speed * delta_time;

			// 剩余移动距离
			auto remaining_move_distance = total_move_distance;

			// 这两个变量挪到此作用域是因为它们仅在跨越网格时变动
			// 当前网格点
			auto current_point = tile_map.coordinate_world_to_grid(position.position);
			// 当前网格中心点
			auto current_point_center_position = tile_map.coordinate_grid_to_world(current_point);

			while (remaining_move_distance > 0)
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
				// 1.此网格流向与上一个网格流向相同
				// 2.此网格流向与上一个网格流向不同,且敌人经过此网格中心点
				// 敌人方向 != 网格流向:
				// 1.此网格流向与上一个网格流向不同,且敌人未经过中心点
				if (const auto flow_direction = flow_field.direction_of(current_point);
					direction.direction == flow_direction)
				{
					// 中心 ==> 边界

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
						const auto next_point_signed = sf::Vector2i{current_point} + direction_value;
						const auto next_point_unsigned = sf::Vector2u{next_point_signed};

						current_point = next_point_unsigned;
						current_point_center_position = tile_map.coordinate_grid_to_world(current_point);
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

					// 到中心点距离
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

					// 此次移动实际移动距离
					const auto current_move_distance = std::ranges::min(remaining_move_distance, max_movable_distance);

					position.position += current_move_distance * direction_normalized_value;
					remaining_move_distance -= current_move_distance;

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
}
