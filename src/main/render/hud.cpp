#include <render/hud.hpp>

#include <components/core/tags.hpp>
#include <components/game/game.hpp>
#include <components/game/wave.hpp>
#include <components/game/player.hpp>
#include <components/game/hud.hpp>
#include <components/map/observer.hpp>

#include <utility/functional.hpp>

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

namespace render
{
	auto hud(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
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

			hud_text.setString(std::format(L"生命值: {} / 魔法值: {}", health, mana));
			hud_text.setPosition({10, static_cast<float>(window_size.y - 120)});
			hud_text.setFillColor(sf::Color::Red);
			window.draw(hud_text);
		}

		// Resource
		{
			const auto gold = player_resource.at(resource::Type::GOLD);

			hud_text.setString(std::format(L"金币: ${}", gold));
			hud_text.setPosition({10, static_cast<float>(window_size.y - 90)});
			hud_text.setFillColor(sf::Color::Yellow);
			window.draw(hud_text);
		}

		// Statistics
		{
			const auto& [frame_delta] = registry.ctx().get<const game::FrameDelta>();
			const auto& [elapsed_time] = registry.ctx().get<const game::ElapsedTime>();
			const auto& [elapsed_simulation_time] = registry.ctx().get<const game::ElapsedSimulationTime>();

			const auto& [ground_enemy, ground_enemy_alive] = registry.ctx().get<const observer::GroundEnemy>();
			const auto& [aerial_enemy, aerial_enemy_alive] = registry.ctx().get<const observer::AerialEnemy>();
			const auto& [killed_enemy] = registry.ctx().get<const player::Statistics>();

			hud_text.setString(std::format(
				L"FPS: {:.3f} | 游戏运行时间: {:.3f}秒(模拟时间: {:.3f}秒) | 存活敌人: 地面:{} | 空中:{} / 已击杀敌人数量: {}",
				1.f / frame_delta.asSeconds(),
				elapsed_time.asSeconds(),
				elapsed_simulation_time.asSeconds(),
				ground_enemy_alive,
				aerial_enemy_alive,
				killed_enemy
			));
			hud_text.setPosition({10, static_cast<float>(window_size.y - 60)});
			hud_text.setFillColor(sf::Color::Cyan);
			window.draw(hud_text);
		}

		// Info
		{
			hud_text.setString(L"鼠标左键: 建造塔 | 鼠标右键: 摧毁塔 | 空格键: 暂停/继续 | TAB键: 加速");
			hud_text.setPosition({10, static_cast<float>(window_size.y - 30)});
			hud_text.setFillColor({180, 180, 180});
			window.draw(hud_text);
		}

		// window.setView(view);
	}
}
