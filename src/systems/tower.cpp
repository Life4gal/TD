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
		using namespace components;

		registry.on_construct<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [type] = reg.get<const EntityType>(entity);
				const auto [position] = reg.get<const Position>(entity);

				std::println(
					"[{}] Build tower {}({}) at ({:.0f}:{:.0f})",
					std::chrono::system_clock::now(),
					std::to_underlying(entity),
					std::to_underlying(type),
					position.x,
					position.y
				);
			}
		>();

		registry.on_destroy<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Destroy tower {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}
		>();

		sf::CircleShape shape{};
		{
			const auto& [map] = registry.ctx().get<const map_ex::Map>();

			shape.setRadius(static_cast<float>(std::ranges::min(map.tile_width(), map.tile_height())) * .35f);
			shape.setOrigin({shape.getRadius(), shape.getRadius()});
			shape.setFillColor({200, 150, 50});
		}

		render::Tower render_tower
		{
				.shape = std::move(shape),
		};

		registry.ctx().emplace<render::Tower>(std::move(render_tower));
	}

	auto Tower::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = delta;
	}

	auto Tower::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		if (auto* render_tower = registry.ctx().find<render::Tower>())
		{
			auto& shape = render_tower->shape;

			for (const auto tower_view = registry.view<tags::tower, Position>();
			     const auto [entity, position]: tower_view.each())
			{
				shape.setPosition(position.position);

				window.draw(shape);
			}
		}
	}

	auto Tower::build(entt::registry& registry, const sf::Vector2u grid_position, const components::EntityType tower_type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [map] = registry.ctx().get<const map_ex::Map>();
		auto& map_counter = registry.ctx().get<map_ex::Counter>();

		// 检查资源是否足够以及位置是否合法在外部进行
		assert(map.inside(grid_position.x, grid_position.y) and map.at(grid_position.x, grid_position.y) == map::TileType::TOWER);
		const auto position = map.coordinate_grid_to_world(grid_position);

		const auto entity = registry.create();

		registry.emplace<EntityType>(entity, tower_type);
		registry.emplace<Position>(entity, position);

		// todo: 读取配置文件
		{
			registry.emplace<tower::Modifier>(entity, 0);
			registry.emplace<tower::Equipment>(entity);
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::tower>(entity);

		map_counter.built_tower += 1;
		return entity;
	}
}
