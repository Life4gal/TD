#include <render/renderable.hpp>

#include <components/core/tags.hpp>
#include <components/core/transform.hpp>
#include <components/core/renderable.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace render
{
	auto renderable(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		for (const auto entity_view = registry.view<
			     Renderable,
			     const transform::Position,
			     const transform::Scale,
			     const transform::Rotation>(entt::exclude<tags::invisible>);
		     const auto [entity, renderable, position, scale, rotation]: entity_view.each())
		{
			auto& sprite = renderable.sprite;

			sprite.setPosition(position.position);
			sprite.setScale(scale.scale);
			sprite.setRotation(rotation.rotation);

			window.draw(sprite);
		}
	}
}
