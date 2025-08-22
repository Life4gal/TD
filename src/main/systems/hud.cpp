#include <systems/hud.hpp>

#include <print>

#include <components/hud.hpp>
#include <components/map.hpp>
#include <components/observer.hpp>
#include <components/player.hpp>

#include <components/font.hpp>

#include <systems/enemy.hpp>
#include <systems/resource.hpp>
#include <systems/wave.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <imgui.h>

namespace systems
{
	auto HUD::initialize(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [fonts] = registry.ctx().get<Fonts>();

		const auto [it, result] = fonts.load(constants::hud, "hud");
		assert(result);
		assert(it->second);

		registry.ctx().emplace<hud::Render>(sf::Text{it->second, "", 25});
	}

	auto HUD::update(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		using entity_underlying_type = entity::type_underlying_type;
		constexpr entity_underlying_type enemy_type_base = 0x1000;
		constexpr entity_underlying_type tower_type_base = 0x2000;

		const auto& [map_start_gates, map_end_gates] = registry.ctx().get<const map_ex::Gate>();

		auto& [player_selected_tower_type] = registry.ctx().get<player::Interaction>();

		ImGui::Begin("辅助窗口");

		{
			{
				ImGui::Text("波次管理");
				ImGui::Separator();

				const auto& [waves, durations] = registry.ctx().get<const wave::WaveSequence>();

				if (ImGui::Button("立即开始当前波次"))
				{
					Wave::start(registry);
				}

				ImGui::SeparatorText("生成");
				for (std::size_t wave_index = 0; wave_index < waves.size(); ++wave_index)
				{
					if (const auto label = std::format("第 {} 波##spawn", wave_index);
						ImGui::Button(label.c_str()))
					{
						Wave::spawn(registry, static_cast<wave::WaveIndex>(wave_index));
					}
				}

				ImGui::SeparatorText("跳转");
				for (std::size_t wave_index = 0; wave_index < waves.size(); ++wave_index)
				{
					if (const auto label = std::format("第 {} 波##start_at", wave_index);
						ImGui::Button(label.c_str()))
					{
						Wave::start_at(registry, static_cast<wave::WaveIndex>(wave_index));
					}
				}
			}
			{
				static entity_underlying_type selected_enemy_type = std::to_underlying(entity::invalid_type);

				ImGui::Text("选择敌人");
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
						if (selected_enemy_type == std::to_underlying(entity::invalid_type))
						{
							std::println("未选择敌人类型");
						}
						else
						{
							const auto type = static_cast<entity::Type>(selected_enemy_type);

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
						player_selected_tower_type = {static_cast<entity::Type>(tower_type_base + i)};

						std::println("选择塔: {}", std::to_underlying(player_selected_tower_type));
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
					if (health != 0)
					{
						Resource::acquire(
							registry,
							resource::Resource{resource::Type::HEALTH, static_cast<resource::size_type>(health)}
						);
					}
					if (mana != 0)
					{
						Resource::acquire(
							registry,
							resource::Resource{resource::Type::MANA, static_cast<resource::size_type>(mana)}
						);
					}
					if (gold != 0)
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

		const auto& [player_resource] = registry.ctx().get<const player::Resource>();

		auto& [hud_text] = registry.ctx().get<hud::Render>();

		// const auto& view = window.getView();
		// window.setView(window.getDefaultView());

		const auto window_size = window.getSize();

		// Wave
		{
			const auto [wave_duration] = registry.ctx().get<const wave::WaveDuration>();
			const auto wave_index = registry.ctx().get<const wave::WaveIndex>();
			const auto wave_index_value = std::to_underlying(wave_index);

			if (Wave::has_next_wave(registry))
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

		// window.setView(view);
	}
}
