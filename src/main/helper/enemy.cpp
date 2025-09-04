#include <helper/enemy.hpp>

#include <random>

#include <components/map.hpp>
#include <components/navigation.hpp>

#include <components/tags.hpp>
#include <components/enemy.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Enemy::spawn(entt::registry& registry, const sf::Vector2u point, const components::entity::Type type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();

		const auto position = tile_map.coordinate_grid_to_world(point);

		const auto entity = registry.create();

		registry.emplace<entity::Type>(entity, type);
		registry.emplace<entity::Position>(entity, position);

		// todo: 加载配置文件
		{
			static std::mt19937 random{std::random_device{}()};

			// 地面单位
			registry.emplace<tags::archetype_ground>(entity);
			const auto direction = flow_field.direction_of(point);
			registry.emplace<enemy::Direction>(entity, direction);

			// 生命值
			auto& [health] = registry.emplace<enemy::Health>(entity);
			health = 100;

			// 移动速度
			auto& [speed] = registry.emplace<enemy::Movement>(entity);
			speed = 30.f + std::uniform_real_distribution<float>{0, 20}(random);

			// 强度
			auto& [power] = registry.emplace<enemy::Power>(entity);
			power = 1 + std::uniform_int_distribution<enemy::Power::value_type>{0, 100}(random);

			sf::CircleShape shape{};
			{
				shape.setRadius(static_cast<float>(tile_map.tile_width()) * .35f);
				shape.setOrigin({shape.getRadius(), shape.getRadius()});
				shape.setFillColor(sf::Color::Red);
			}

			registry.emplace<enemy::Render>(entity, std::move(shape));
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::enemy>(entity);

		return entity;
	}

	auto Enemy::spawn(entt::registry& registry, const std::uint32_t start_gate_id, const components::entity::Type type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();
		const auto start_gate = start_gates[start_gate_id];

		return spawn(registry, start_gate, type);
	}
}
