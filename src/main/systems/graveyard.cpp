#include <systems/graveyard.hpp>

#include <components/player.hpp>
#include <components/graveyard.hpp>

#include <components/tags.hpp>

#include <helper/resource.hpp>

#include <entt/entt.hpp>

namespace systems
{
	auto Graveyard::update(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		// 所有死亡实体(不一定只有敌人)
		const auto dead_view = registry.view<tags::dead>();

		// 击杀奖励
		{
			// 被杀死的实体(只有敌人吗?)
			const auto killed_view = registry.view<tags::enemy_killed>();

			if (const auto killed_count = killed_view.size();
				killed_count != 0)
			{
				auto& [killed_enemy] = registry.ctx().get<player::Statistics>();

				for (const auto [entity]: killed_view.each())
				{
					helper::Resource::acquire(registry, entity);
					killed_enemy += 1;
				}
			}
		}

		registry.destroy(dead_view.begin(), dead_view.end());
	}
}
