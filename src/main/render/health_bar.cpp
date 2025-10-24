#include <render/health_bar.hpp>

#include <components/core/tags.hpp>
#include <components/core/transform.hpp>
#include <components/combat/enemy.hpp>
#include <components/combat/health_bar.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace render
{
	auto health_bar(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		const auto enemy_view = registry.view<
			const enemy::Health,
			const health_bar::Health,
			const health_bar::Size,
			const health_bar::Offset,
			const transform::Position>(entt::exclude<tags::dead>);
		const auto enemy_count = enemy_view.size_hint();

		// 2个矩形 * 6个顶点(每个矩形2个三角形)
		sf::VertexArray triangles{sf::PrimitiveType::Triangles, enemy_count * 12};

		for (const auto [entity, current_health, max_health, hb_size, hb_offset, position]: enemy_view.each())
		{
			// 血条比率
			const auto ratio = current_health.health / max_health.health;
			// 血条颜色
			constexpr auto hb_color = sf::Color::Red;
			const auto h_color = ratio > .6f ? sf::Color::Green : sf::Color::Yellow;

			const auto hb_position = position.position + hb_offset.offset;
			const auto h_size = sf::Vector2f{hb_size.size.x * ratio, hb_size.size.y};

			// 背景矩形

			triangles.append({.position = hb_position, .color = hb_color, .texCoords = {}});
			triangles.append({.position = hb_position + sf::Vector2f{hb_size.size.x, 0}, .color = hb_color, .texCoords = {}});
			triangles.append({.position = hb_position + hb_size.size, .color = hb_color, .texCoords = {}});

			triangles.append({.position = hb_position, .color = hb_color, .texCoords = {}});
			triangles.append({.position = hb_position + hb_size.size, .color = hb_color, .texCoords = {}});
			triangles.append({.position = hb_position + sf::Vector2f{0, hb_size.size.y}, .color = hb_color, .texCoords = {}});

			// 血条矩形

			triangles.append({.position = hb_position, .color = h_color, .texCoords = {}});
			triangles.append({.position = hb_position + sf::Vector2f{h_size.x, 0}, .color = h_color, .texCoords = {}});
			triangles.append({.position = hb_position + h_size, .color = h_color, .texCoords = {}});

			triangles.append({.position = hb_position, .color = h_color, .texCoords = {}});
			triangles.append({.position = hb_position + h_size, .color = h_color, .texCoords = {}});
			triangles.append({.position = hb_position + sf::Vector2f{0, h_size.y}, .color = h_color, .texCoords = {}});
		}

		window.draw(triangles);
		triangles.clear();
	}
}
