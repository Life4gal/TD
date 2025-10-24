#include <update/observer.hpp>

#include <components/core/tags.hpp>
#include <components/core/transform.hpp>
#include <components/map/map.hpp>
#include <components/map/observer.hpp>

#include <entt/entt.hpp>

namespace update
{
	auto observer(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		std::ignore = delta;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		auto& [ground_enemy, ground_enemy_alive] = registry.ctx().get<observer::GroundEnemy>();
		auto& [aerial_enemy, aerial_enemy_alive] = registry.ctx().get<observer::AerialEnemy>();

		const auto ground_view = registry.view<tags::archetype_ground, const transform::Position>(entt::exclude<tags::dead>);
		const auto aerial_view = registry.view<tags::archetype_aerial, const transform::Position>(entt::exclude<tags::dead>);

		ground_enemy_alive = ground_view.size_hint();
		aerial_enemy_alive = aerial_view.size_hint();

		ground_enemy.clear();
		for (const auto [entity, position]: ground_view.each())
		{
			const auto grid_position = tile_map.coordinate_world_to_grid(position.position);

			ground_enemy[grid_position].push_back(entity);
		}

		aerial_enemy.clear();
		for (const auto [entity, position]: aerial_view.each())
		{
			const auto grid_position = tile_map.coordinate_world_to_grid(position.position);

			aerial_enemy[grid_position].push_back(entity);
		}
	}
}
