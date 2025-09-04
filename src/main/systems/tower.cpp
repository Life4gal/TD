#include <systems/tower.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/tower.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Tower::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		for (const auto tower_view = registry.view<tower::Render, const entity::Position>(entt::exclude<tags::invisible>);
		     const auto [entity, shape, position]: tower_view.each())
		{
			shape.shape.setPosition(position.position);

			window.draw(shape.shape);
		}
	}
}
