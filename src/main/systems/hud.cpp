#include <systems/hud.hpp>

#include <print>

#include <components/map.hpp>
#include <components/observer.hpp>

#include <components/tags.hpp>
#include <components/player.hpp>
#include <components/hud.hpp>

#include <helper/wave.hpp>
#include <helper/resource.hpp>
#include <helper/enemy.hpp>

#include <utility/functional.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <imgui.h>

namespace systems
{
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

				const auto [wave_total_count] = registry.ctx().get<const wave::WaveTotalCount>();

				if (ImGui::Button("游戏开始"))
				{
					helper::Wave::start_from_wave(registry, {.index = 0});
				}

				using sm = helper::Wave::StateMachine;

				if (ImGui::Button("跳过准备时间"))
				{
					sm::on(registry, sm::SkipPreparationRequested{});
				}

				if (ImGui::Button("立即开始下一波次"))
				{
					sm::on(registry, sm::EarlyStartRequested{});
				}

				ImGui::SeparatorText("生成");
				for (std::size_t wave_index = 0; wave_index < wave_total_count; ++wave_index)
				{
					if (const auto label = std::format("第 {} 波##spawn", wave_index);
						ImGui::Button(label.c_str()))
					{
						helper::Wave::spawn_wave(registry, {.index = static_cast<wave::index_type>(wave_index)});
					}
				}

				ImGui::SeparatorText("跳转");
				for (std::size_t wave_index = 0; wave_index < wave_total_count; ++wave_index)
				{
					if (const auto label = std::format("第 {} 波##start_at", wave_index);
						ImGui::Button(label.c_str()))
					{
						helper::Wave::start_from_wave(registry, {.index = static_cast<wave::index_type>(wave_index)});
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

							helper::Enemy::spawn(registry, static_cast<std::uint32_t>(i), type);
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
						helper::Resource::acquire(
							registry,
							resource::Resource{resource::Type::HEALTH, static_cast<resource::size_type>(health)}
						);
					}
					if (mana != 0)
					{
						helper::Resource::acquire(
							registry,
							resource::Resource{resource::Type::MANA, static_cast<resource::size_type>(mana)}
						);
					}
					if (gold != 0)
					{
						helper::Resource::acquire(
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

		auto& [hud_text] = registry.ctx().get<hud::Text>();

		// const auto& view = window.getView();
		// window.setView(window.getDefaultView());

		const auto window_size = window.getSize();

		// Wave
		{
			if (const auto [wave_current_index] = registry.ctx().get<const wave::WaveCurrentIndex>();
				wave_current_index == wave::wave_not_start)
			{
				hud_text.setString(L"按下'辅助窗口'中的'游戏开始'以开始");
			}
			else if (wave_current_index == wave::wave_all_completed)
			{
				hud_text.setString(L"所有波次已完成");
			}
			else
			{
				const auto [wave_current_entity] = registry.ctx().get<const wave::WaveCurrentEntity>();
				assert(registry.valid(wave_current_entity));

				// 等待阶段单独处理
				if (const auto state = registry.get<const wave::WaveState>(wave_current_entity);
					state == wave::WaveState::PREPARING)
				{
					// 如果波次没有准备时间,则其可能处于等待阶段(波次刚创建,还没有更新过)且没有准备时间计时器
					if (const auto* preparation_timer = registry.try_get<const wave::WavePreparationTimer>(wave_current_entity))
					{
						hud_text.setString(std::format(
							L"波次 [{}] | 准备阶段 (剩余{:.3f}秒...",
							wave_current_index,
							preparation_timer->remaining.asSeconds()
						));
					}
					else
					{
						// 其实我们完全可以什么都不显示,因为该分支可能仅持续1帧
						hud_text.setString(std::format(L"波次 [{}]", wave_current_index));
					}
				}
				else
				{
					const auto& [end_condition] = registry.get<const wave::EndCondition>(wave_current_entity);

					const auto get_alive_enemy = [&](const entt::entity wave_entity) noexcept -> std::size_t
					{
						const auto& [wave_enemy] = registry.get<const wave::WaveEnemy>(wave_entity);
						const auto wave_alive_enemy = std::ranges::count_if(
							wave_enemy,
							[&](const entt::entity enemy) noexcept -> bool
							{
								return registry.valid(enemy) and not registry.all_of<tags::dead>(enemy);
							}
						);

						return wave_alive_enemy;
					};

					std::visit(
						utility::overloads{
								[&](const wave::EndCondition::Extinction&) noexcept -> void
								{
									const auto total_alive = [&]
									{
										const auto wave_view = registry.view<tags::wave>();

										return std::ranges::fold_left(
											wave_view,
											std::size_t{0},
											[&](const std::size_t total, const entt::entity wave_entity) noexcept -> std::size_t
											{
												return total + get_alive_enemy(wave_entity);
											}
										);
									}();

									hud_text.setString(std::format(
										L"波次 [{}] (剩余{}个敌人)",
										wave_current_index,
										total_alive
									));
								},
								[&](const wave::EndCondition::Clear&) noexcept -> void
								{
									const auto alive = get_alive_enemy(wave_current_entity);

									hud_text.setString(std::format(
										L"波次 [{}] (剩余{}个敌人)",
										wave_current_index,
										alive
									));
								},
								[&](const wave::EndCondition::Duration& condition) noexcept -> void
								{
									const auto alive = get_alive_enemy(wave_current_entity);

									const auto& [elapsed_time] = registry.get<const wave::WaveTimer>(wave_current_entity);
									const auto remaining = condition.duration - elapsed_time;

									hud_text.setString(std::format(
										L"波次 [{}] (剩余{}个敌人) | {:.3f}秒后开始下一波次",
										wave_current_index,
										alive,
										remaining.asSeconds()
									));
								},
								[&](const wave::EndCondition::Type& condition) noexcept -> void
								{
									const auto alive = get_alive_enemy(wave_current_entity);

									hud_text.setString(std::format(
										L"波次 [{}] (剩余{}个敌人) | 消灭所有({})类型的敌人",
										wave_current_index,
										alive,
										std::to_underlying(condition.type)
									));
								},
						},
						end_condition
					);
				}
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
