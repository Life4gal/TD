#include <systems/hud.hpp>

#include <print>

#include <components/render.hpp>

#include <components/map.hpp>
#include <components/player.hpp>

#include <systems/enemy.hpp>
#include <systems/resource.hpp>

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

	auto HUD::update(entt::registry& registry) noexcept -> void
	{
		using entity_underlying_type = std::underlying_type_t<components::EntityType::Type>;
		constexpr entity_underlying_type enemy_type_base = 0;
		constexpr entity_underlying_type tower_type_base = 1000;

		const auto& map_data = registry.ctx().get<components::MapData>();
		auto& player_data = registry.ctx().get<components::PlayerData>();

		ImGui::Begin("辅助窗口");
		{
			{
				static entity_underlying_type selected_enemy_type = std::to_underlying(components::EntityType::invalid_type);

				ImGui::Text("选择敌人类型");
				ImGui::Separator();

				for (entity_underlying_type i = 0; i < 3; ++i)
				{
					if (const auto label = std::format("敌人 {}", i);
						ImGui::Button(label.c_str()))
					{
						selected_enemy_type = enemy_type_base + i;

						std::println("选择敌人: {}", selected_enemy_type);
					}
				}

				ImGui::Text("生成敌人");
				ImGui::Separator();

				for (std::size_t i = 0; i < map_data.start_gates.size(); ++i)
				{
					if (const auto label = std::format("出生点 {}", i);
						ImGui::Button(label.c_str()))
					{
						if (selected_enemy_type == std::to_underlying(components::EntityType::invalid_type))
						{
							std::println("未选择敌人类型");
						}
						else
						{
							const components::EntityType type{static_cast<components::EntityType::Type>(selected_enemy_type)};

							Enemy::spawn(registry, static_cast<std::uint32_t>(i), type);
						}
					}
				}
			}

			{
				ImGui::Text("选择塔");
				ImGui::Separator();

				for (entity_underlying_type i = 0; i < 3; ++i)
				{
					if (const auto label = std::format("塔 {}", i);
						ImGui::Button(label.c_str()))
					{
						player_data.selected_tower_type = {static_cast<components::EntityType::Type>(tower_type_base + i)};

						std::println("选择塔: {}", std::to_underlying(player_data.selected_tower_type.type));
					}
				}
			}

			{
				ImGui::Text("Show me the money");
				ImGui::Separator();

				static int health = 0;
				static int mana = 0;
				static int gold = 0;
				ImGui::InputInt("Health", &health);
				ImGui::InputInt("Mana", &mana);
				ImGui::InputInt("Gold", &gold);

				if (ImGui::Button("Acquire"))
				{
					if (health > 0)
					{
						Resource::acquire(
							registry,
							components::Resource{components::ResourceType::HEALTH, static_cast<components::Resource::size_type>(health)}
						);
					}
					if (mana > 0)
					{
						Resource::acquire(
							registry,
							components::Resource{components::ResourceType::MANA, static_cast<components::Resource::size_type>(mana)}
						);
					}
					if (gold > 0)
					{
						Resource::acquire(
							registry,
							components::Resource{components::ResourceType::GOLD, static_cast<components::Resource::size_type>(gold)}
						);
					}
				}
			}
		}
		ImGui::End();
	}

	auto HUD::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		if (auto* render_hud_data = registry.ctx().find<components::RenderHUDData>())
		{
			const auto& map_data = registry.ctx().get<components::MapData>();
			const auto& player_data = registry.ctx().get<components::PlayerData>();
			const auto& player_resources = player_data.resources;

			const auto window_size = window.getSize();
			auto& hud_text = render_hud_data->hud_text;

			// Health & Mana
			{
				const auto health = player_resources.at(components::ResourceType::HEALTH);
				const auto mana = player_resources.at(components::ResourceType::MANA);

				hud_text.setString(std::format("Health: {} / Mana: {}", health, mana));
				hud_text.setPosition({10, static_cast<float>(window_size.y - 120)});
				hud_text.setFillColor(sf::Color::Red);
				window.draw(hud_text);
			}

			// Resources
			{
				const auto gold = player_resources.at(components::ResourceType::GOLD);

				hud_text.setString(std::format("Gold: ${}", gold));
				hud_text.setPosition({10, static_cast<float>(window_size.y - 90)});
				hud_text.setFillColor(sf::Color::Yellow);
				window.draw(hud_text);
			}

			// Counter
			{
				hud_text.setString(std::format("Enemies: {}, Enemy Kills: {}, Towers: {}", map_data.enemy_counter, map_data.enemy_kill_counter, map_data.tower_counter));
				hud_text.setPosition({10, static_cast<float>(window_size.y - 60)});
				hud_text.setFillColor(sf::Color::Blue);
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
