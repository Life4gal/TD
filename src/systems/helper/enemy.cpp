#include <systems/helper/enemy.hpp>

#include <random>

#include <components/tags.hpp>
#include <components/enemy.hpp>
#include <components/map.hpp>

#include <entt/entt.hpp>

namespace systems::helper
{
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
			static std::mt19937 random{std::random_device{}()};

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
