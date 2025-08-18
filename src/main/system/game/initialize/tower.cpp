#include <system/game/initialize/tower.hpp>

#include <chrono>
#include <print>

#include <component/game/tags.hpp>
#include <component/game/entity.hpp>
#include <component/game/tower.hpp>
#include <component/game/render.hpp>
#include <component/game/map.hpp>

#include <entt/entt.hpp>

namespace game::system::initialize
{
	auto tower(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		registry.on_construct<tags::tower::identifier>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto type = reg.get<const entity::Type>(entity);
				const auto [position] = reg.get<const entity::Position>(entity);

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

		registry.on_destroy<tags::tower::identifier>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Destroy tower {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}
		>();

		sf::CircleShape shape{};
		{
			const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

			shape.setRadius(static_cast<float>(std::ranges::min(tile_map.tile_width(), tile_map.tile_height())) * .35f);
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
