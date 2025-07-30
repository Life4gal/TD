#include <systems/graveyard.hpp>

#include <components/tags.hpp>
#include <components/graveyard.hpp>

#include <components/map.hpp>

#include <systems/resource.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Graveyard::initialize(entt::registry& registry) noexcept -> void
	{
		std::ignore = registry;
	}

	auto Graveyard::update(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& map_counter = registry.ctx().get<map_ex::Counter>();

		const auto enemy_killed_view = registry.view<tags::enemy, tags::enemy_killed>();
		const auto enemy_reached_view = registry.view<tags::enemy, tags::enemy_reached>();

		const auto killed_count = enemy_killed_view.size_hint();
		const auto reached_count = enemy_reached_view.size_hint();

		// 击杀敌人获取资源
		if (killed_count != 0)
		{
			std::vector<EntityType> types{};
			types.reserve(killed_count);

			std::ranges::transform(
				enemy_killed_view,
				std::back_inserter(types),
				[&](const entt::entity entity) noexcept -> EntityType
				{
					return registry.get<const EntityType>(entity);
				}
			);

			Resource::acquire(registry, types);
		}

		registry.destroy(enemy_killed_view.begin(), enemy_killed_view.end());
		registry.destroy(enemy_reached_view.begin(), enemy_reached_view.end());

		map_counter.alive_enemy -= static_cast<std::uint32_t>(killed_count + reached_count);
		map_counter.killed_enemy += static_cast<std::uint32_t>(killed_count);
	}

	auto Graveyard::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = window;
	}
}
