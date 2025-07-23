#include <systems/enemy.hpp>

#include <random>
#include <chrono>
#include <print>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/enemy.hpp>
#include <components/render.hpp>

#include <components/map.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace
{
	std::mt19937 random{std::random_device{}()};
}

namespace systems
{
	auto Enemy::initialize(entt::registry& registry) noexcept -> void
	{
		registry.on_construct<components::tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [type] = reg.get<components::EntityType>(entity);
				const auto [position] = reg.get<components::WorldPosition>(entity);

				std::println(
					"[{}] Spawn enemy {}({}) at ({:.0f}:{:.0f})",
					std::chrono::system_clock::now(),
					std::to_underlying(entity),
					std::to_underlying(type),
					position.x,
					position.y
				);
			}
		>();

		registry.on_destroy<components::tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Kill enemy {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}
		>();

		sf::CircleShape shape{};
		{
			const auto& map_data = registry.ctx().get<components::MapData>();
			const auto& map = map_data.map;

			shape.setRadius(static_cast<float>(std::ranges::min(map.tile_width(), map.tile_height())) * .35f);
			shape.setOrigin({shape.getRadius(), shape.getRadius()});
			shape.setFillColor(sf::Color::Red);
		}

		components::RenderEnemyData render_enemy_data
		{
				.shape = std::move(shape),
		};

		registry.ctx().emplace<components::RenderEnemyData>(std::move(render_enemy_data));
	}

	auto Enemy::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = delta;
	}

	auto Enemy::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		if (auto* render_enemy_data = registry.ctx().find<components::RenderEnemyData>())
		{
			for (const auto enemy_view = registry.view<components::tags::enemy, components::tags::enemy_alive, components::WorldPosition>();
			     const auto [entity, position]: enemy_view.each())
			{
				render_enemy_data->shape.setPosition(position.position);

				window.draw(render_enemy_data->shape);
			}
		}
	}

	auto Enemy::spawn(entt::registry& registry, const sf::Vector2u point, const components::EntityType enemy_type) noexcept -> entt::entity
	{
		auto& map_data = registry.ctx().get<components::MapData>();
		const auto& map = map_data.map;

		const auto entity = registry.create();

		registry.emplace<components::tags::enemy_alive>(entity);
		registry.emplace<components::EntityType>(entity, enemy_type);

		auto& [position] = registry.emplace<components::WorldPosition>(entity);
		position = map.coordinate_grid_to_world(point);

		// todo: 加载配置文件
		{
			auto& [speed] = registry.emplace<components::Movement>(entity);
			speed = 30.f + std::uniform_real_distribution<float>{0, 20}(random);

			auto& [health] = registry.emplace<components::Health>(entity);
			health = 100;
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<components::tags::enemy>(entity);

		map_data.enemy_counter += 1;
		return entity;
	}

	auto Enemy::spawn(entt::registry& registry, const std::uint32_t start_gate_id, const components::EntityType enemy_type) noexcept -> entt::entity
	{
		const auto& map_data = registry.ctx().get<components::MapData>();
		const auto start_gate = map_data.start_gates[start_gate_id];

		return spawn(registry, start_gate, enemy_type);
	}
}
