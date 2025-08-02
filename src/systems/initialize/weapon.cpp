#include <systems/initialize/weapon.hpp>

#include <chrono>
#include <print>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/weapon.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>

namespace systems::initialize
{
	auto weapon(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::weapon>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [type] = reg.get<const EntityType>(entity);
				const auto [owner] = reg.get<const weapon::Owner>(entity);

				std::println(
					"[{}] Equip weapon {}({}) to {}",
					std::chrono::system_clock::now(),
					std::to_underlying(entity),
					std::to_underlying(type),
					std::to_underlying(owner)
				);
			}
		>();

		registry.on_destroy<tags::weapon>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Remove weapon {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}
		>();

		// render
		{
			render::Weapon render_weapon
			{
					.target_line = sf::VertexArray{sf::PrimitiveType::Lines},
			};

			registry.ctx().emplace<render::Weapon>(std::move(render_weapon));
		}

		//
	}
}
