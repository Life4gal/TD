#include <systems/navigation.hpp>

#include <components/enemy.hpp>
#include <components/render.hpp>

#include <components/map.hpp>
#include <components/navigation.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Navigation::initialize(entt::registry& registry) noexcept -> void
	{
		//
	}

	auto Navigation::update(entt::registry& registry, sf::Time delta) noexcept -> void
	{
		//
	}

	auto Navigation::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		if (auto* render_navigation_data = registry.ctx().find<components::RenderNavigationData>())
		{
			const auto& map_data = registry.ctx().get<components::MapData>();
			const auto& map = map_data.map;

			const auto& navigation_data = registry.ctx().get<components::NavigationData>();
			const auto& cache_paths = navigation_data.cache_paths;

			auto& path_vertices = render_navigation_data->path_vertices;

			std::ranges::for_each(
				cache_paths,
				[&](const auto& path) noexcept -> void
				{
					std::ranges::for_each(
						path,
						[&](const auto point) noexcept -> void
						{
							const auto world_position = map.coordinate_grid_to_world(point);

							path_vertices.append({.position = world_position, .color = sf::Color::Green, .texCoords = {}});
						}
					);

					window.draw(path_vertices);
					path_vertices.clear();
				}
			);
		}
	}
}
