#include <system/game/render/enemy.hpp>

#include <component/game/tags.hpp>
#include <component/game/entity.hpp>
#include <component/game/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace game::system::render
{
	auto enemy(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace component;

		if (auto* render_enemy = registry.ctx().find<component::render::Enemy>())
		{
			auto& shape = render_enemy->shape;

			for (const auto enemy_view = registry.view<tags::enemy::identifier, const entity::Position>();
			     const auto [entity, position]: enemy_view.each())
			{
				shape.setPosition(position.position);

				window.draw(shape);
			}
		}
	}
}
