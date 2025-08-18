#include <system/game/render/hud.hpp>

#include <component/game/wave.hpp>
#include <component/game/resource.hpp>
#include <component/game/player.hpp>
#include <component/game/observer.hpp>
#include <component/game/render.hpp>

#include <system/game/helper/wave.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace game::system::render
{
	auto hud(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace component;

		if (auto* render_hud = registry.ctx().find<component::render::HUD>())
		{
			const auto& [player_resource] = registry.ctx().get<const player::Resource>();

			const auto window_size = window.getSize();
			auto& hud_text = render_hud->hud_text;

			// Wave
			{
				const auto [wave_duration] = registry.ctx().get<const wave::WaveDuration>();
				const auto wave_index = registry.ctx().get<const wave::WaveIndex>();
				const auto wave_index_value = std::to_underlying(wave_index);

				if (helper::Wave::has_next_wave(registry))
				{
					hud_text.setString(std::format("Next Wave: {} | Next Wave Time: {:.3f}", wave_index_value, wave_duration.asSeconds()));
				}
				else
				{
					hud_text.setString(std::format("Next Wave: {} (Last Wave!)", wave_index_value));
				}

				hud_text.setPosition({10, static_cast<float>(window_size.y - 150)});
				hud_text.setFillColor(sf::Color::Green);
				window.draw(hud_text);
			}

			// Health & Mana
			{
				const auto health = player_resource.at(resource::Type::HEALTH);
				const auto mana = player_resource.at(resource::Type::MANA);

				hud_text.setString(std::format("Health: {} / Mana: {}", health, mana));
				hud_text.setPosition({10, static_cast<float>(window_size.y - 120)});
				hud_text.setFillColor(sf::Color::Red);
				window.draw(hud_text);
			}

			// Resource
			{
				const auto gold = player_resource.at(resource::Type::GOLD);

				hud_text.setString(std::format("Gold: ${}", gold));
				hud_text.setPosition({10, static_cast<float>(window_size.y - 90)});
				hud_text.setFillColor(sf::Color::Yellow);
				window.draw(hud_text);
			}

			// Statistics
			{
				const auto& [ground_alive, aerial_alive] = registry.ctx().get<const observer::EnemyStatistics>();
				const auto& [killed_enemy] = registry.ctx().get<const player::Statistics>();

				hud_text.setString(std::format("Alive: G:{} | A:{} / Killed: {}", ground_alive, aerial_alive, killed_enemy));
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
