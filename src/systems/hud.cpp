#include <systems/hud.hpp>

#include <components/render.hpp>

#include <components/map.hpp>

#include <systems/enemy.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <imgui.h>

namespace systems
{
	auto HUD::initialize(entt::registry& registry) noexcept -> void
	{
		auto& font_manager = manager::Font::instance();
		const auto hud_font_id = font_manager.precache(R"(C:\Windows\Fonts\msyh.ttc)");
		assert(hud_font_id.has_value());

		components::RenderHUDData render_hud_data
		{
				.hud_font = *hud_font_id,
				.hud_text = {font_manager.font_of(*hud_font_id), "", 25},
		};

		registry.ctx().emplace<components::RenderHUDData>(std::move(render_hud_data));
	}

	auto HUD::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		std::ignore = delta;

		{
			const auto& map_data = registry.ctx().get<components::MapData>();

			ImGui::Begin("生成敌人");

			for (std::size_t i = 0; i < map_data.start_gates.size(); ++i)
			{
				if (const auto label = std::format("出生点 {}", i);
					ImGui::Button(label.c_str()))
				{
					Enemy::spawn(registry, static_cast<std::uint32_t>(i));
				}
			}

			ImGui::End();
		}
	}

	auto HUD::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		if (auto* render_hud_data = registry.ctx().find<components::RenderHUDData>())
		{
			const auto& map_data = registry.ctx().get<components::MapData>();

			const auto window_size = window.getSize();
			auto& hud_text = render_hud_data->hud_text;

			hud_text.setString(std::format("Enemies: {}, Enemy Kills: {}, Towers: {}", map_data.enemy_counter, map_data.enemy_kill_counter, map_data.tower_counter));
			hud_text.setPosition({10, static_cast<float>(window_size.y - 60)});
			hud_text.setFillColor(sf::Color::White);
			window.draw(hud_text);

			hud_text.setString("SPACE: PAUSE/UNPAUSE, TAB: ACCELERATE");
			hud_text.setPosition({10, static_cast<float>(window_size.y - 30)});
			hud_text.setFillColor(sf::Color::White);
			window.draw(hud_text);
		}
	}
}
