#include <system/game/update/graveyard.hpp>

#include <component/game/tags.hpp>
#include <component/game/player.hpp>
#include <component/game/graveyard.hpp>

#include <system/game/helper/resource.hpp>

#include <entt/entt.hpp>

namespace game::system::update
{
	auto graveyard(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		const auto enemy_dead_view = registry.view<tags::enemy::identifier, tags::enemy::status::dead>();
		const auto enemy_reached_view = registry.view<tags::enemy::identifier, tags::enemy::status::reached>();

		// 击杀奖励
		if (const auto dead_count = enemy_dead_view.size_hint(); dead_count != 0)
		{
			for (const auto [entity]: enemy_dead_view.each())
			{
				auto& [killed_enemy] = registry.ctx().get<player::Statistics>();

				helper::Resource::acquire(registry, entity);
				killed_enemy += 1;
			}
		}

		registry.destroy(enemy_dead_view.begin(), enemy_dead_view.end());
		registry.destroy(enemy_reached_view.begin(), enemy_reached_view.end());
	}
}
