#include <render/map.hpp>

#include <components/map/map.hpp>

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

namespace render
{
	auto map(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		const auto& [sprite] = registry.ctx().get<map_ex::Background>();

		window.draw(sprite);
	}
}
