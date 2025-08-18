#include <system/game/render/weapon.hpp>

#include <component/game/tags.hpp>
#include <component/game/entity.hpp>
#include <component/game/weapon.hpp>
#include <component/game/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace game::system::render
{
	auto weapon(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace component;

		if (auto* render_weapon = registry.ctx().find<component::render::Weapon>())
		{
			auto& target_line = render_weapon->target_line;

			for (const auto weapon_view = registry.view<tags::weapon::identifier, const weapon::Owner, const weapon::Target>();
			     const auto [entity, owner, target]: weapon_view.each())
			{
				// 目标可能已经被别的塔击杀
				// if (not registry.all_of<tags::enemy_alive>(target.entity))
				if (not registry.valid(target.entity))
				{
					continue;
				}

				const auto [tower_position] = registry.get<const entity::Position>(owner.entity);
				const auto [enemy_position] = registry.get<const entity::Position>(target.entity);

				target_line.append({.position = tower_position, .color = sf::Color::Red, .texCoords = {}});
				target_line.append({.position = enemy_position, .color = sf::Color::Red, .texCoords = {}});
			}

			window.draw(target_line);
			target_line.clear();
		}
	}
}
