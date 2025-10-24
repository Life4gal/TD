#include <render/renderable.hpp>

#include <print>

#include <components/core/tags.hpp>
#include <components/core/transform.hpp>
#include <components/core/renderable.hpp>
#include <components/combat/unit.hpp>

#include <helper/asset.hpp>

#include <utility/time.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace render
{
	auto renderable(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		static sf::RenderStates states{};

		const auto renderable_view = registry.view<
			const transform::Position,
			const transform::Scale,
			const transform::Rotation,
			const renderable::Texture,
			const renderable::Area,
			const renderable::Origin,
			const renderable::Color>(entt::exclude<tags::invisible>);

		for (const auto [entity, position, scale, rotation, texture, area, origin, color]: renderable_view.each())
		{
			auto render_texture = helper::Asset::texture_of(registry, texture.id);
			if (not render_texture)
			{
				const auto name = [&]
				{
					if (const auto* n = registry.try_get<combat::Name>(entity))
					{
						return n->name;
					}

					return std::format("ENTITY {}", entt::to_integral(entity));
				}();

				std::print(
					"[{:%Y-%m-%d %H:%M:%S}] [RENDERABLE] 无法渲染位于({:.3f}:{:.3f})的实体[{}],纹理ID: {}",
					utility::zoned_now(),
					position.position.x,
					position.position.y,
					name,
					texture.id
				);
			}

			auto vertices = [&]
			{
				const auto rect = sf::FloatRect{area.area};
				const auto left = rect.position.x;
				const auto top = rect.position.y;
				const auto right = left + rect.size.x;
				const auto bottom = top + rect.size.y;

				std::array<sf::Vertex, 4> vs{};
				vs[0] = {.position = {0, 0}, .color = color.color, .texCoords = {left, top}};
				vs[1] = {.position = {rect.size.x, 0}, .color = color.color, .texCoords = {right, top}};
				vs[2] = {.position = {0, rect.size.y}, .color = color.color, .texCoords = {left, bottom}};
				vs[3] = {.position = {rect.size.x, rect.size.y}, .color = color.color, .texCoords = {right, bottom}};

				return vs;
			}();

			states.texture = render_texture.operator->();
			states.transform = [&]
			{
				sf::Transform t{};
				t.translate(position.position);
				t.scale(scale.scale);
				t.rotate(rotation.rotation);
				t.translate(-origin.origin);

				return t;
			}();

			window.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TriangleStrip, states);
		}
	}
}
