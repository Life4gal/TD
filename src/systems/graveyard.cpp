#include <systems/graveyard.hpp>

#include <components/tags.hpp>

#include <components/map.hpp>

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
		auto& map_data = registry.ctx().get<components::MapData>();

		const auto enemy_killed_view = registry.view<components::tags::enemy, components::tags::enemy_killed>();
		const auto enemy_reached_view = registry.view<components::tags::enemy, components::tags::enemy_reached>();

		const auto killed_count = enemy_killed_view.size_hint();
		const auto reached_count = enemy_reached_view.size_hint();

		// todo: 击杀敌人获取资源

		registry.destroy(enemy_killed_view.begin(), enemy_killed_view.end());
		registry.destroy(enemy_reached_view.begin(), enemy_reached_view.end());

		map_data.enemy_counter -= static_cast<std::uint32_t>(killed_count + reached_count);
		map_data.enemy_kill_counter += static_cast<std::uint32_t>(killed_count);
	}

	auto Graveyard::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = window;
	}
}
