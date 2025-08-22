#include <systems/map.hpp>

#include <components/map.hpp>

#include <components/texture.hpp>

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

namespace systems
{
	auto Map::load(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		using size_type = map::TileMap::size_type;

		std::vector start_gates{sf::Vector2u{3, 3}, sf::Vector2u{26, 3}};
		std::vector end_gates{sf::Vector2u{4, 23}, sf::Vector2u{28, 23}};

		// TileMap
		{
			constexpr size_type tile_width = 32;
			constexpr size_type tile_height = 32;
			constexpr size_type horizontal_tile_count = 40;
			constexpr size_type vertical_tile_count = 30;

			map::TileMap tile_map{tile_width, tile_height, horizontal_tile_count, vertical_tile_count};
			{
				using enum map::TileType;

				std::ranges::fill(tile_map, BUILDABLE_FLOOR);

				for (size_type y = 14; y <= 19; ++y)
				{
					for (size_type x = 1; x <= 3; ++x)
					{
						tile_map.set(x, y, OBSTACLE);
					}
				}

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

		// Render
		{
			auto& [textures] = registry.ctx().get<Textures>();

			// 载入地图时
			// constants::map, loaders::TextureType::MAP
			// 两个参数不变,constants::map指定纹理id为地图,loaders::TextureType::MAP指示纹理类型为地图
			// 第三个参数为地图文件文件名(不含后缀),这个按照具体关卡指定
			auto [it, result] = textures.load(constants::map, loaders::TextureType::MAP, "map1");
			assert(result);
			assert(it != textures.end());

			registry.ctx().emplace<map_ex::Render>(sf::Sprite{it->second});

			// todo: 现在将起点/终点视为地图的一部分,也就是说起点/终点已经位于地图上,无需额外绘制
			// 是否支持一张地图但是起点/终点位于不同位置?
		}
	}

	auto Map::initialize(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		std::ignore = registry;
	}

	auto Map::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		const auto& [sprite] = registry.ctx().get<map_ex::Render>();

		window.draw(sprite);
	}
}
