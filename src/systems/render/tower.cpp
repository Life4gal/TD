#include <systems/render/tower.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/tower.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems::render
{
	auto tower(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		if (auto* render_tower = registry.ctx().find<components::render::Tower>())
		{
			auto& shape = render_tower->shape;

			for (const auto tower_view = registry.view<tags::tower, Position>();
			     const auto [entity, position]: tower_view.each())
			{
				shape.setPosition(position.position);

				window.draw(shape);
			}
		}
	}
}
