#include <systems/update/resource.hpp>

#include <components/tags.hpp>
#include <components/player.hpp>

#include <entt/entt.hpp>

namespace systems::update
{
	auto resource(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [player_resources] = registry.ctx().get<player::Resource>();

		const auto resource_reward_view = registry.view<tags::resource, resource::Resource>();

		for (const auto [entity, resource]: resource_reward_view.each())
		{
			player_resources[resource.type()] += resource.count();
		}

		registry.destroy(resource_reward_view.begin(), resource_reward_view.end());
	}
}
