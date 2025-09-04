#include <systems/enemy.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/enemy.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Enemy::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		for (const auto enemy_view = registry.view<enemy::Render, const entity::Position>(entt::exclude<tags::invisible>);
		     const auto [entity, shape, position]: enemy_view.each())
		{
			shape.shape.setPosition(position.position);

			window.draw(shape.shape);
		}
	}
}
