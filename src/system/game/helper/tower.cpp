#include <system/game/helper/tower.hpp>

#include <component/game/tags.hpp>
#include <component/game/tower.hpp>
#include <component/game/map.hpp>

#include <entt/entt.hpp>

namespace game::system::helper
{
	auto Tower::build(entt::registry& registry, const sf::Vector2u grid_position, const component::entity::Type tower_type) noexcept -> entt::entity
	{
		using namespace component;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		// 检查资源是否足够以及位置是否合法在外部进行
		assert(tile_map.inside(grid_position.x, grid_position.y) and tile_map.at(grid_position.x, grid_position.y) == map::TileType::TOWER);
		const auto position = tile_map.coordinate_grid_to_world(grid_position);

		const auto entity = registry.create();

		registry.emplace<entity::Type>(entity, tower_type);
		registry.emplace<entity::Position>(entity, position);

		// todo: 读取配置文件
		{
			registry.emplace<tower::Modifier>(entity, 0);
			registry.emplace<tower::Equipment>(entity);
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::tower::identifier>(entity);

		return entity;
	}
}
