#include <systems/helper/tower.hpp>

#include <components/tags.hpp>
#include <components/tower.hpp>
#include <components/map.hpp>

#include <entt/entt.hpp>

namespace systems::helper
{
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
