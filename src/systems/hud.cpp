#include <systems/hud.hpp>

#include <print>

#include <components/hud.hpp>
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
		using namespace components;

		auto& font_manager = manager::Font::instance();
		const auto hud_font_id = font_manager.precache(R"(C:\Windows\Fonts\msyh.ttc)");
		assert(hud_font_id.has_value());

		render::HUD render_hud
		{
				.hud_font = *hud_font_id,
				.hud_text = {font_manager.font_of(*hud_font_id), "", 25},
		};

		registry.ctx().emplace<render::HUD>(std::move(render_hud));
	}

	auto HUD::update(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		using entity_underlying_type = std::underlying_type_t<EntityType::Type>;
		constexpr entity_underlying_type enemy_type_base = 0;
		constexpr entity_underlying_type tower_type_base = 1000;

		const auto& [map_start_gates, map_end_gates] = registry.ctx().get<const map_ex::Gate>();

		auto& [player_selected_tower_type, player_selected_weapon_type] = registry.ctx().get<player::Interaction>();

		ImGui::Begin("辅助窗口");
		{
			{
				static entity_underlying_type selected_enemy_type = std::to_underlying(EntityType::invalid_type);

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

				for (std::size_t i = 0; i < map_start_gates.size(); ++i)
				{
					if (const auto label = std::format("出生点 {}", i);
						ImGui::Button(label.c_str()))
					{
						if (selected_enemy_type == std::to_underlying(EntityType::invalid_type))
						{
							std::println("未选择敌人类型");
						}
						else
						{
							const EntityType type{static_cast<EntityType::Type>(selected_enemy_type)};

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
						player_selected_tower_type = {static_cast<EntityType::Type>(tower_type_base + i)};

						std::println("选择塔: {}", std::to_underlying(player_selected_tower_type.type));
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
							resource::Resource{resource::Type::HEALTH, static_cast<resource::size_type>(health)}
						);
					}
					if (mana > 0)
					{
						Resource::acquire(
							registry,
							resource::Resource{resource::Type::MANA, static_cast<resource::size_type>(mana)}
						);
					}
					if (gold > 0)
					{
						Resource::acquire(
							registry,
							resource::Resource{resource::Type::GOLD, static_cast<resource::size_type>(gold)}
						);
					}
				}
			}
		}
		ImGui::End();
	}

	auto HUD::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		if (auto* render_hud = registry.ctx().find<render::HUD>())
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
