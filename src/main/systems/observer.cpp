#include <systems/observer.hpp>

#include <components/map.hpp>
#include <components/observer.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>

#include <entt/entt.hpp>

namespace systems
{
	auto Observer::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		using size_type = observer::EnemyStatistics::size_type;

		std::ignore = delta;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		auto& [enemies_ground, enemies_aerial] = registry.ctx().get<observer::EnemyArchetype>();
		auto& [ground_alive, aerial_alive] = registry.ctx().get<observer::EnemyStatistics>();

		const auto enemy_ground_view = registry.view<tags::archetype_ground, tags::enemy, const entity::Position>(entt::exclude<tags::dead>);
		const auto enemy_aerial_view = registry.view<tags::archetype_aerial, tags::enemy, const entity::Position>(entt::exclude<tags::dead>);

		ground_alive = static_cast<size_type>(enemy_ground_view.size_hint());
		aerial_alive = static_cast<size_type>(enemy_aerial_view.size_hint());

		enemies_ground.clear();
		for (const auto [entity, position]: enemy_ground_view.each())
		{
			const auto grid_position = tile_map.coordinate_world_to_grid(position.position);

			enemies_ground[grid_position].push_back(entity);
		}

		enemies_aerial.clear();
		for (const auto [entity, position]: enemy_aerial_view.each())
		{
			const auto grid_position = tile_map.coordinate_world_to_grid(position.position);

			enemies_aerial[grid_position].push_back(entity);
		}
	}
}
