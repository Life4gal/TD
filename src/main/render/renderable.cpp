#include <render/renderable.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/renderable.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace render
{
	auto renderable(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		for (const auto entity_view = registry.view<Renderable, const entity::Position, const entity::Scale>(entt::exclude<tags::invisible>);
		     const auto [entity, renderable, position, scale]: entity_view.each())
		{
			auto& sprite = renderable.sprite;

			sprite.setPosition(position.position);
			sprite.setScale(scale.scale);

			window.draw(sprite);
		}
	}
}
