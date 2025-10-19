#include <systems/resource.hpp>

#include <components/resource.hpp>
#include <components/player.hpp>

#include <helper/entity.hpp>

#include <entt/entt.hpp>

namespace systems
{
	auto Resource::update(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [player_resource] = registry.ctx().get<player::Resource>();

		const auto resource_view = registry.view<const resource::Resource>();

		for (const auto [entity, resource]: resource_view.each())
		{
			player_resource[resource.type()] += resource.amount();
		}

		helper::Entity::kill(registry, resource_view.begin(), resource_view.end());
	}
}
