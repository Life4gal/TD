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

		if (auto* render = registry.ctx().find<tower::DebugRenderTarget>())
		{
			auto& lines = render->lines;

			for (const auto tower_view = registry.view<const tower::Target, tags::tower>();
			     const auto [entity, target]: tower_view.each())
			{
				// 目标可能已经被别的塔击杀
				// if (registry.all_of<tags::dead>(target.entity))
				if (not registry.valid(target.entity))
				{
					continue;
				}

				const auto [tower_position] = registry.get<const entity::Position>(entity);
				const auto [enemy_position] = registry.get<const entity::Position>(target.entity);

				lines.append({.position = tower_position, .color = sf::Color::Red, .texCoords = {}});
				lines.append({.position = enemy_position, .color = sf::Color::Red, .texCoords = {}});
			}

			window.draw(lines);
			lines.clear();
		}
	}
}
