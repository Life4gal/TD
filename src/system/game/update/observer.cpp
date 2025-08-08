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

		using size_type = observer::EnemyStatistics::size_type;

		std::ignore = delta;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		auto& [enemies_ground, enemies_aerial] = registry.ctx().get<observer::EnemyArchetype>();
		auto& [ground_alive, aerial_alive] = registry.ctx().get<observer::EnemyStatistics>();

		const auto ground_enemy_view = registry.view<tags::enemy::identifier, tags::enemy::status::alive, tags::enemy::archetype::ground, const entity::Position>();
		const auto aerial_enemy_view = registry.view<tags::enemy::identifier, tags::enemy::status::alive, tags::enemy::archetype::aerial, const entity::Position>();

		ground_alive = static_cast<size_type>(ground_enemy_view.size_hint());
		aerial_alive = static_cast<size_type>(aerial_enemy_view.size_hint());

		enemies_ground.clear();
		for (const auto [entity, position]: ground_enemy_view.each())
		{
			const auto grid_position = tile_map.coordinate_world_to_grid(position.position);

			enemies_ground[grid_position].push_back(entity);
		}

		enemies_aerial.clear();
		for (const auto [entity, position]: aerial_enemy_view.each())
		{
			const auto grid_position = tile_map.coordinate_world_to_grid(position.position);

			enemies_aerial[grid_position].push_back(entity);
		}
	}
}
