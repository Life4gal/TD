#include <systems/map.hpp>

#include <components/map.hpp>

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

namespace systems
{
	auto Map::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		const auto& [sprite] = registry.ctx().get<map_ex::Background>();

		window.draw(sprite);
	}
}
