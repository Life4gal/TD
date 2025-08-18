#include <system/game/render/tower.hpp>

#include <component/game/tags.hpp>
#include <component/game/entity.hpp>
#include <component/game/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace game::system::render
{
	auto tower(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace component;

		if (auto* render_tower = registry.ctx().find<component::render::Tower>())
		{
			auto& shape = render_tower->shape;

			for (const auto tower_view = registry.view<tags::tower::identifier, entity::Position>();
			     const auto [entity, position]: tower_view.each())
			{
				shape.setPosition(position.position);

				window.draw(shape);
			}
		}
	}
}
