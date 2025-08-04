#include <system/game/update/hud.hpp>

#include <print>

#include <component/game/entity.hpp>
#include <component/game/player.hpp>
#include <component/game/map.hpp>

#include <system/game/helper/enemy.hpp>
#include <system/game/helper/resource.hpp>

#include <entt/entt.hpp>

#include <imgui.h>

namespace game::system::update
{
	auto hud(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		using entity_underlying_type = entity::type_underlying_type;
		constexpr entity_underlying_type enemy_type_base = 0;
		constexpr entity_underlying_type tower_type_base = 1000;
		constexpr entity_underlying_type weapon_type_base = 2000;

		const auto& [map_start_gates, map_end_gates] = registry.ctx().get<const map_ex::Gate>();

		auto& [player_selected_tower_type, player_selected_weapon_type] = registry.ctx().get<player::Interaction>();

		ImGui::Begin("辅助窗口");
		{
			{
				static entity_underlying_type selected_enemy_type = std::to_underlying(entity::invalid_type);

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
				ImGui::Text("选择武器");
				ImGui::Separator();

				for (entity_underlying_type i = 0; i < 3; ++i)
				{
					if (const auto label = std::format("武器 {}", i);
						ImGui::Button(label.c_str()))
					{
						player_selected_weapon_type = {static_cast<entity::Type>(weapon_type_base + i)};

						std::println("选择武器: {}", std::to_underlying(player_selected_weapon_type));
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
}
