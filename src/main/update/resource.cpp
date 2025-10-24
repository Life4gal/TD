#include <update/resource.hpp>

#include <components/game/player.hpp>

#include <entt/entt.hpp>

namespace update
{
	auto resource(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [player_resource] = registry.ctx().get<player::Resource>();

		const auto resource_view = registry.view<const resource::Resource>();

		for (const auto [entity, resource]: resource_view.each())
		{
			player_resource[resource.type()] += resource.amount();
		}

		registry.destroy(resource_view.begin(), resource_view.end());
	}
}
