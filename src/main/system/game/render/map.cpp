#include <system/game/render/map.hpp>

#include <component/game/map.hpp>
#include <component/game/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace game::system::render
{
	auto map(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace component;

		if (auto* render_map = registry.ctx().find<component::render::Map>())
		{
			const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();
			const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();

			// 绘制瓦片
			{
				using size_type = map::TileMap::size_type;

				auto& tile_shape = render_map->tile_shape;

				for (size_type y = 0; y < tile_map.vertical_tile_count(); ++y)
				{
					for (size_type x = 0; x < tile_map.horizontal_tile_count(); ++x)
					{
						tile_shape.setPosition({static_cast<float>(x * tile_map.tile_width()), static_cast<float>(y * tile_map.tile_height())});

						using enum map::TileType;
						switch (tile_map.at(x, y))
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
					const auto position = tile_map.coordinate_grid_to_world(gate);
					gate_shape.setPosition(position);

					window.draw(gate_shape);
				}

				gate_shape.setFillColor(sf::Color::Red);
				for (const auto gate: end_gates)
				{
					const auto position = tile_map.coordinate_grid_to_world(gate);
					gate_shape.setPosition(position);

					window.draw(gate_shape);
				}
			}
		}
	}
}
