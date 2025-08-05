#include <system/game/update/observer.hpp>

#include <component/game/tags.hpp>
#include <component/game/entity.hpp>
#include <component/game/map.hpp>
#include <component/game/observer.hpp>

#include <entt/entt.hpp>

namespace game::system::update
{
	auto observer(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace component;

		std::ignore = delta;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		auto& [enemies_ground, enemies_aerial] = registry.ctx().get<observer::EnemyArchetype>();
		auto& [enemy_alive] = registry.ctx().get<observer::EnemyStatistics>();

		enemies_ground.clear();
		enemies_aerial.clear();
		enemy_alive = 0;

		for (const auto enemy_view = registry.view<tags::enemy::identifier, tags::enemy::status::alive, const entity::Position>();
		     const auto [entity, position]: enemy_view.each())
		{
			enemy_alive += 1;

			if (registry.all_of<tags::enemy::status::visible>(entity))
			{
				const auto grid_position = tile_map.coordinate_world_to_grid(position.position);

				if (registry.all_of<tags::enemy::archetype::ground>(entity))
				{
					enemies_ground[grid_position].push_back(entity);
				}
				else if (registry.all_of<tags::enemy::archetype::aerial>(entity))
				{
					enemies_ground[grid_position].push_back(entity);
				}
				else
				{
					// 要么地面移动,要么空中移动,不允许其他情况
					std::unreachable();
				}
			}
		}
	}
}
