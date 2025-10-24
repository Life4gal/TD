#include <update/player.hpp>

#include <components/tags.hpp>
#include <components/player.hpp>

#include <entt/entt.hpp>

namespace update
{
	auto player(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [player_resource] = registry.ctx().get<player::Resource>();
		auto& player_health = player_resource[resource::Type::HEALTH];

		// 到达终点的敌人
		const auto enemy_reached_view = registry.view<tags::cod_reached>();
		const auto reached_count = enemy_reached_view.size();

		// todo: 每个敌人扣多少生命值?
		if (const auto cost_health = static_cast<resource::size_type>(reached_count) * 1;
			cost_health >= player_health)
		{
			player_health = 0;

			// todo: 游戏结束
		}
		else
		{
			player_health -= cost_health;
		}
	}
}
