#include <systems/initialize/enemy.hpp>

#include <chrono>
#include <print>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/enemy.hpp>
#include <components/map.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>

namespace systems::initialize
{
	auto enemy(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [type] = reg.get<const EntityType>(entity);
				const auto [position] = reg.get<const Position>(entity);

				std::println(
					"[{}] Spawn enemy {}({}) at ({:.0f}:{:.0f})",
					std::chrono::system_clock::now(),
					std::to_underlying(entity),
					std::to_underlying(type),
					position.x,
					position.y
				);
			}
		>();

		registry.on_destroy<tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Kill enemy {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}
		>();

		sf::CircleShape shape{};
		{
			const auto& [map] = registry.ctx().get<const map_ex::Map>();

			shape.setRadius(static_cast<float>(std::ranges::min(map.tile_width(), map.tile_height())) * .35f);
			shape.setOrigin({shape.getRadius(), shape.getRadius()});
			shape.setFillColor(sf::Color::Red);
		}

		render::Enemy render_enemy
		{
				.shape = std::move(shape),
		};

		registry.ctx().emplace<render::Enemy>(std::move(render_enemy));
	}
}
