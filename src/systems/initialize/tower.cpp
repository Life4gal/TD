#include <systems/initialize/tower.hpp>

#include <chrono>
#include <print>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/map.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>

namespace systems::initialize
{
	auto tower(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [type] = reg.get<const EntityType>(entity);
				const auto [position] = reg.get<const Position>(entity);

				std::println(
					"[{}] Build tower {}({}) at ({:.0f}:{:.0f})",
					std::chrono::system_clock::now(),
					std::to_underlying(entity),
					std::to_underlying(type),
					position.x,
					position.y
				);
			}
		>();

		registry.on_destroy<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Destroy tower {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}
		>();

		sf::CircleShape shape{};
		{
			const auto& [map] = registry.ctx().get<const map_ex::Map>();

			shape.setRadius(static_cast<float>(std::ranges::min(map.tile_width(), map.tile_height())) * .35f);
			shape.setOrigin({shape.getRadius(), shape.getRadius()});
			shape.setFillColor({200, 150, 50});
		}

		render::Tower render_tower
		{
				.shape = std::move(shape),
		};

		registry.ctx().emplace<render::Tower>(std::move(render_tower));
	}
}
