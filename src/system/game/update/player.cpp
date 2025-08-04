#include <system/game/update/player.hpp>

#include <component/game/tags.hpp>
#include <component/game/player.hpp>

#include <entt/entt.hpp>

namespace game::system::update
{
	auto player(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		auto& [player_resource] = registry.ctx().get<player::Resource>();
		auto& player_health = player_resource[resource::Type::HEALTH];

		const auto enemy_reached_view = registry.view<tags::enemy::identifier, tags::enemy::status::reached>();
		const auto reached_count = enemy_reached_view.size_hint();

		// todo: 每个敌人扣多少生命值?
		if (const auto cost_health = static_cast<resource::size_type>(reached_count) * 1;
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
