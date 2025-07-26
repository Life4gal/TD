#include <systems/tower.hpp>

#include <print>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/tower.hpp>
#include <components/render.hpp>

#include <components/map.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Tower::initialize(entt::registry& registry) noexcept -> void
	{
		registry.on_construct<components::tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [type] = reg.get<components::EntityType>(entity);
				const auto [position] = reg.get<components::GridPosition>(entity);

				std::println(
					"[{}] Build tower {}({}) at ({}:{})",
					std::chrono::system_clock::now(),
					std::to_underlying(entity),
					std::to_underlying(type),
					position.x,
					position.y
				);
			}
		>();

		registry.on_destroy<components::tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Destroy tower {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}
		>();

		sf::CircleShape shape{};
		{
			const auto& map_data = registry.ctx().get<components::MapData>();
			const auto& map = map_data.map;

			shape.setRadius(static_cast<float>(std::ranges::min(map.tile_width(), map.tile_height())) * .35f);
			shape.setOrigin({shape.getRadius(), shape.getRadius()});
			shape.setFillColor({200, 150, 50});
		}

		components::RenderTowerData render_tower_data
		{
				.shape = std::move(shape),
		};

		registry.ctx().emplace<components::RenderTowerData>(std::move(render_tower_data));
	}

	auto Tower::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = delta;
	}

	auto Tower::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		if (auto* render_tower_data = registry.ctx().find<components::RenderTowerData>())
		{
			const auto& map_data = registry.ctx().get<components::MapData>();
			const auto& map = map_data.map;

			for (const auto tower_view = registry.view<components::tags::tower, components::GridPosition>();
			     const auto [entity, position]: tower_view.each())
			{
				const auto world_position = map.coordinate_grid_to_world(position.position);
				render_tower_data->shape.setPosition(world_position);

				window.draw(render_tower_data->shape);
			}
		}
	}

	auto Tower::build(entt::registry& registry, const sf::Vector2u grid_position, const components::EntityType tower_type) noexcept -> entt::entity
	{
		auto& map_data = registry.ctx().get<components::MapData>();

		// 检查资源是否足够和位置是否合法在外部进行
		assert(map_data.map.inside(grid_position.x, grid_position.y) and map_data.map.at(grid_position.x, grid_position.y) == map::TileType::TOWER);

		const auto entity = registry.create();

		registry.emplace<components::EntityType>(entity, tower_type);
		registry.emplace<components::GridPosition>(entity, grid_position);

		// todo: 读取配置文件
		{
			registry.emplace<components::Modifier>(entity, 0);
			registry.emplace<components::Equipment>(entity);
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<components::tags::tower>(entity);

		map_data.tower_counter += 1;
		return entity;
	}
}
