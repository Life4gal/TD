#include <update/graveyard.hpp>

#include <components/core/tags.hpp>
#include <components/game/player.hpp>
#include <components/game/graveyard.hpp>

#include <helper/resource.hpp>

#include <entt/entt.hpp>

namespace update
{
	auto graveyard(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		// 所有死亡实体(不一定只有敌人)
		const auto dead_view = registry.view<tags::dead>();

		// 击杀奖励
		{
			// 被杀死的实体(只有敌人吗?)
			const auto killed_view = registry.view<tags::cod_killed>();

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
