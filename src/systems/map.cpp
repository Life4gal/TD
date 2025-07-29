#include <systems/map.hpp>

#include <components/map.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Map::initialize(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		using size_type = map::TileMap::size_type;

		// todo: 加载配置文件
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

		// render
		{
			render::Map render_map
			{
					.tile_shape = sf::RectangleShape{{static_cast<float>(tile_map.tile_width()), static_cast<float>(tile_map.tile_height())},},
					.gate_shape = sf::CircleShape{static_cast<float>(std::ranges::min(tile_map.tile_width(), tile_map.tile_height())) * .45f},
			};

			// tile
			{
				auto& tile_shape = render_map.tile_shape;

				tile_shape.setOutlineColor(sf::Color::Red);
				tile_shape.setOutlineThickness(1.f);
			}
			// gate
			{
				auto& gate_shape = render_map.gate_shape;

				gate_shape.setOrigin({gate_shape.getRadius(), gate_shape.getRadius()});
			}

			registry.ctx().emplace<render::Map>(std::move(render_map));
		}

		registry.ctx().emplace<map_ex::Map>(std::move(tile_map));
		registry.ctx().emplace<map_ex::Gate>(std::move(start_gates), std::move(end_gates));
		registry.ctx().emplace<map_ex::Counter>(map_ex::Counter{.alive_enemy = 0, .killed_enemy = 0, .built_tower = 0});
	}

	auto Map::update(entt::registry& registry) noexcept -> void
	{
		std::ignore = registry;
	}

	auto Map::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		if (auto* render_map = registry.ctx().find<render::Map>())
		{
			const auto& [map] = registry.ctx().get<const map_ex::Map>();
			const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();

			// 绘制瓦片
			{
				using size_type = map::TileMap::size_type;

				auto& tile_shape = render_map->tile_shape;

				for (size_type y = 0; y < map.vertical_tile_count(); ++y)
				{
					for (size_type x = 0; x < map.horizontal_tile_count(); ++x)
					{
						tile_shape.setPosition({static_cast<float>(x * map.tile_width()), static_cast<float>(y * map.tile_height())});

						using enum map::TileType;
						switch (map.at(x, y))
						{
							case FLOOR:
							{
								tile_shape.setFillColor({70, 70, 70});
								break;
							}
							case BUILDABLE_FLOOR:
							{
								tile_shape.setFillColor({40, 90, 40});
								break;
							}
							case OBSTACLE:
							{
								tile_shape.setFillColor({20, 20, 20});
								break;
							}
							case TOWER:
							{
								tile_shape.setFillColor({120, 70, 40});
								break;
							}
						}

						window.draw(tile_shape);
					}
				}
			}

			// 绘制起点和终点
			{
				auto& gate_shape = render_map->gate_shape;

				gate_shape.setFillColor(sf::Color::Green);
				for (const auto gate: start_gates)
				{
					const auto position = map.coordinate_grid_to_world(gate);
					gate_shape.setPosition(position);

					window.draw(gate_shape);
				}

				gate_shape.setFillColor(sf::Color::Red);
				for (const auto gate: end_gates)
				{
					const auto position = map.coordinate_grid_to_world(gate);
					gate_shape.setPosition(position);

					window.draw(gate_shape);
				}
			}
		}
	}
}
