#include <system/game/helper/enemy.hpp>

#include <random>

#include <component/game/tags.hpp>
#include <component/game/enemy.hpp>
#include <component/game/map.hpp>

#include <entt/entt.hpp>

namespace game::system::helper
{
	auto Enemy::spawn(entt::registry& registry, const sf::Vector2u point, const component::entity::Type enemy_type) noexcept -> entt::entity
	{
		using namespace component;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		const auto position = tile_map.coordinate_grid_to_world(point);

		const auto entity = registry.create();

		registry.emplace<tags::enemy::status::alive>(entity);
		registry.emplace<tags::enemy::status::visible>(entity);

		registry.emplace<entity::Type>(entity, enemy_type);
		registry.emplace<entity::Position>(entity, position);

		// todo: 加载配置文件
		{
			static std::mt19937 random{std::random_device{}()};

			registry.emplace<tags::enemy::archetype::ground>(entity);
			registry.emplace<enemy::Power>(entity, static_cast<enemy::Power::value_type>(1));

			auto& [speed] = registry.emplace<enemy::Movement>(entity);
			speed = 30.f + std::uniform_real_distribution<float>{0, 20}(random);

			auto& [health] = registry.emplace<enemy::Health>(entity);
			health = 100;
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::enemy::identifier>(entity);

		return entity;
	}

	auto Enemy::spawn(entt::registry& registry, const std::uint32_t start_gate_id, const component::entity::Type enemy_type) noexcept -> entt::entity
	{
		using namespace component;

		const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();
		const auto start_gate = start_gates[start_gate_id];

		return spawn(registry, start_gate, enemy_type);
	}
}
