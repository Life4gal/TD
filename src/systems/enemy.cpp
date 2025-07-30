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
		using namespace components;

		registry.on_construct<tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [type] = reg.get<const EntityType>(entity);
				const auto [position] = reg.get<const Position>(entity);

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

		registry.on_destroy<tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Kill enemy {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}
		>();

		sf::CircleShape shape{};
		{
			const auto& [map] = registry.ctx().get<const map_ex::Map>();

			shape.setRadius(static_cast<float>(std::ranges::min(map.tile_width(), map.tile_height())) * .35f);
			shape.setOrigin({shape.getRadius(), shape.getRadius()});
			shape.setFillColor(sf::Color::Red);
		}

		render::Enemy render_enemy
		{
				.shape = std::move(shape),
		};

		registry.ctx().emplace<render::Enemy>(std::move(render_enemy));
	}

	auto Enemy::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = delta;
	}

	auto Enemy::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		if (auto* render_enemy = registry.ctx().find<render::Enemy>())
		{
			auto& shape = render_enemy->shape;

			for (const auto enemy_view = registry.view<tags::enemy, tags::enemy_alive, Position>();
			     const auto [entity, position]: enemy_view.each())
			{
				shape.setPosition(position.position);

				window.draw(shape);
			}
		}
	}

	auto Enemy::spawn(entt::registry& registry, const sf::Vector2u point, const components::EntityType enemy_type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [map] = registry.ctx().get<const map_ex::Map>();
		auto& map_counter = registry.ctx().get<map_ex::Counter>();

		const auto entity = registry.create();

		registry.emplace<tags::enemy_alive>(entity);
		registry.emplace<tags::enemy_aimable>(entity);
		registry.emplace<EntityType>(entity, enemy_type);

		auto& [position] = registry.emplace<Position>(entity);
		position = map.coordinate_grid_to_world(point);

		// todo: 加载配置文件
		{
			registry.emplace<enemy::Category>(entity, enemy::CategoryValue::GROUND);
			registry.emplace<enemy::Power>(entity, static_cast<enemy::Power::value_type>(1));

			auto& [speed] = registry.emplace<enemy::Movement>(entity);
			speed = 30.f + std::uniform_real_distribution<float>{0, 20}(random);

			auto& [health] = registry.emplace<enemy::Health>(entity);
			health = 100;
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::enemy>(entity);

		map_counter.alive_enemy += 1;
		return entity;
	}

	auto Enemy::spawn(entt::registry& registry, const std::uint32_t start_gate_id, const components::EntityType enemy_type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();
		const auto start_gate = start_gates[start_gate_id];

		return spawn(registry, start_gate, enemy_type);
	}
}
