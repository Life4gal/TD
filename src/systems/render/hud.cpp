#include <systems/render/hud.hpp>

#include <components/player.hpp>
#include <components/map.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems::render
{
	auto hud(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		if (auto* render_hud = registry.ctx().find<components::render::HUD>())
		{
			const auto& [map_alive_enemy, map_killed_enemy, map_built_tower] = registry.ctx().get<const map_ex::Counter>();

			const auto& [player_resources] = registry.ctx().get<const player::Resource>();

			const auto window_size = window.getSize();
			auto& hud_text = render_hud->hud_text;

			// Health & Mana
			{
				const auto health = player_resources.at(resource::Type::HEALTH);
				const auto mana = player_resources.at(resource::Type::MANA);

				hud_text.setString(std::format("Health: {} / Mana: {}", health, mana));
				hud_text.setPosition({10, static_cast<float>(window_size.y - 120)});
				hud_text.setFillColor(sf::Color::Red);
				window.draw(hud_text);
			}

			// Resources
			{
				const auto gold = player_resources.at(resource::Type::GOLD);

				hud_text.setString(std::format("Gold: ${}", gold));
				hud_text.setPosition({10, static_cast<float>(window_size.y - 90)});
				hud_text.setFillColor(sf::Color::Yellow);
				window.draw(hud_text);
			}

			// Counter
			{
				hud_text.setString(std::format("Enemies: {} / Enemy Kills: {} / Towers: {}", map_alive_enemy, map_killed_enemy, map_built_tower));
				hud_text.setPosition({10, static_cast<float>(window_size.y - 60)});
				hud_text.setFillColor(sf::Color::Cyan);
				window.draw(hud_text);
			}

			// Info
			{
				hud_text.setString("LMB: Build Tower | RMB: Destroy Tower | SPACE: PAUSE/UNPAUSE | TAB: ACCELERATE");
				hud_text.setPosition({10, static_cast<float>(window_size.y - 30)});
				hud_text.setFillColor({180, 180, 180});
				window.draw(hud_text);
			}
		}
	}
}
