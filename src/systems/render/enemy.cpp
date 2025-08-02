#include <systems/render/enemy.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/enemy.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems::render
{
	auto enemy(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		if (auto* render_enemy = registry.ctx().find<components::render::Enemy>())
		{
			auto& shape = render_enemy->shape;

			for (const auto enemy_view = registry.view<tags::enemy, tags::enemy_alive, Position>();
			     const auto [entity, position]: enemy_view.each())
			{
				shape.setPosition(position.position);

				window.draw(shape);
			}
		}
	}
}
