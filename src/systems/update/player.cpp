#include <systems/update/player.hpp>

#include <components/tags.hpp>
#include <components/player.hpp>

#include <entt/entt.hpp>

namespace systems::update
{
	auto player(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [player_resources] = registry.ctx().get<player::Resource>();
		auto& player_health = player_resources[resource::Type::HEALTH];

		const auto enemy_reached_view = registry.view<tags::enemy, tags::enemy_reached>();
		const auto reached_count = enemy_reached_view.size_hint();

		// todo: 每个敌人扣多少生命值?
		if (const auto cost_health = reached_count * 1;
			cost_health >= player_health)
		{
			// todo: 游戏结束
		}
		else
		{
			player_health -= cost_health;
		}
	}
}
