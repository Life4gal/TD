#include <systems/weapon.hpp>

#include <print>

#include <components/tags.hpp>
#include <components/weapon.hpp>
#include <components/tower.hpp>
#include <components/enemy.hpp>
#include <components/weapon.hpp>
#include <components/render.hpp>

#include <components/map.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace
{
	auto update_cooldown(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		const auto delta_time = delta.asSeconds();

		for (const auto weapon_view = registry.view<tags::weapon, tags::weapon_cooldown, weapon::Specification>();
		     auto [entity, weapon]: weapon_view.each())
		{
			weapon.fire_timer += delta_time;

			if (weapon.fire_timer < weapon.fire_rate)
			{
				// 冷却还没好
				continue;
			}

			// 冷却结束
			weapon.fire_timer -= weapon.fire_rate;
			registry.remove<tags::weapon_cooldown>(entity);
			registry.emplace<tags::weapon_searching>(entity);
		}
	}

	auto update_searching(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		for (const auto weapon_view = registry.view<tags::weapon, tags::weapon_searching, const weapon::Specification, const weapon::Owner>();
		     const auto [entity, weapon, owner]: weapon_view.each())
		{
			assert(registry.valid(owner.entity));

			const auto [owner_position] = registry.get<const Position>(owner.entity);

			// 索敌
			entt::entity target_enemy = entt::null;
			{
				const auto weapon_range_square = std::powf(weapon.range, 2);

				// todo: 保持目标还是按照索敌策略转火?
				if (const auto* weapon_target = registry.try_get<const weapon::Target>(entity);
					// 目标合法
					weapon_target != nullptr and weapon_target->entity != entt::null and registry.valid(weapon_target->entity) and
					// 目标存活
					registry.all_of<tags::enemy_alive>(weapon_target->entity)
				)
				{
					const auto [enemy_position] = registry.get<const Position>(weapon_target->entity);

					if (const auto distance_square = std::powf(owner_position.x - enemy_position.x, 2) + std::powf(owner_position.y - enemy_position.y, 2);
						distance_square <= weapon_range_square)
					{
						// 目标在攻击范围内,进入攻击状态
						registry.remove<tags::weapon_searching>(entity);
						registry.emplace<tags::weapon_attacking>(entity);
						continue;
					}
				}

				// 如果没有目标或者目标不合法,重新寻找目标
				if (weapon.search_strategy == weapon::SearchStrategy::DISTANCE_FIRST)
				{
					auto min_distance_square = weapon_range_square;

					for (const auto enemy_view = registry.view<tags::enemy, tags::enemy_alive, tags::enemy_aimable, const Position, const enemy::Category>();
					     const auto [enemy_entity, enemy_position, enemy_category]: enemy_view.each())
					{
						if (std::to_underlying(enemy_category.category) & weapon.search_type)
						{
							if (const auto distance_square = std::powf(owner_position.x - enemy_position.position.x, 2) + std::powf(owner_position.y - enemy_position.position.y, 2);
								distance_square < min_distance_square)
							{
								min_distance_square = distance_square;
								target_enemy = enemy_entity;
							}
						}
					}
				}
				else if (weapon.search_strategy == weapon::SearchStrategy::POWER_FIRST)
				{
					const auto min_distance_square = weapon_range_square;
					auto max_power = std::numeric_limits<enemy::Power::value_type>::min();

					for (const auto enemy_view = registry.view<tags::enemy, tags::enemy_alive, tags::enemy_aimable, const Position, const enemy::Category, const enemy::Power>();
					     const auto [enemy_entity, enemy_position, enemy_category, enemy_power]: enemy_view.each())
					{
						if (std::to_underlying(enemy_category.category) & weapon.search_type)
						{
							if (const auto distance_square = std::powf(owner_position.x - enemy_position.position.x, 2) + std::powf(owner_position.y - enemy_position.position.y, 2);
								distance_square < min_distance_square and enemy_power.power > max_power)
							{
								max_power = enemy_power.power;
								target_enemy = enemy_entity;
							}
						}
					}
				}
				else
				{
					std::unreachable();
				}
			}

			// 如果能找到一个目标
			if (target_enemy != entt::null)
			{
				registry.emplace_or_replace<weapon::Target>(entity, target_enemy);

				// 进入攻击状态
				registry.remove<tags::weapon_searching>(entity);
				registry.emplace<tags::weapon_attacking>(entity);
			}
			else
			{
				// 找不到目标,保持索敌状态

				// 移除目标组件
				registry.remove<weapon::Target>(entity);
			}
		}
	}

	auto update_fire(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		for (const auto weapon_view = registry.view<tags::weapon, tags::weapon_attacking, const weapon::Specification, const weapon::Target>();
		     const auto [entity, weapon, target]: weapon_view.each())
		{
			weapon.on_fire(registry, entity, target.entity);

			registry.remove<tags::weapon_attacking>(entity);
			registry.emplace<tags::weapon_cooldown>(entity);
		}
	}
}

namespace systems
{
	auto Weapon::initialize(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::weapon>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [type] = reg.get<const EntityType>(entity);
				const auto [owner] = reg.get<const weapon::Owner>(entity);

				std::println(
					"[{}] Equip weapon {}({}) to {}",
					std::chrono::system_clock::now(),
					std::to_underlying(entity),
					std::to_underlying(type),
					std::to_underlying(owner)
				);
			}
		>();

		registry.on_destroy<tags::weapon>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Remove weapon {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}
		>();

		// render
		{
			render::Weapon render_weapon
			{
					.target_line = sf::VertexArray{sf::PrimitiveType::Lines},
			};

			registry.ctx().emplace<render::Weapon>(std::move(render_weapon));
		}

		//
	}

	auto Weapon::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		// ======================================
		// 1.如果武器处于攻击冷却中,检测是否可以攻击(冷却完毕)

		update_cooldown(registry, delta);

		// ======================================
		// 2.如果武器处于索敌状态,寻找目标

		update_searching(registry);

		// ======================================
		// 3.如果武器处于攻击状态,进行攻击

		update_fire(registry);
	}

	auto Weapon::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		if (auto* render_weapon = registry.ctx().find<render::Weapon>())
		{
			auto& target_line = render_weapon->target_line;

			for (const auto weapon_view = registry.view<tags::weapon, const weapon::Owner, const weapon::Target>();
			     const auto [entity, owner, target]: weapon_view.each())
			{
				// 目标可能已经被别的塔击杀
				if (not registry.all_of<tags::enemy_alive>(target.entity))
				{
					continue;
				}

				const auto [tower_position] = registry.get<const Position>(owner.entity);
				const auto [enemy_position] = registry.get<const Position>(target.entity);

				target_line.append({.position = tower_position, .color = sf::Color::Red, .texCoords = {}});
				target_line.append({.position = enemy_position, .color = sf::Color::Red, .texCoords = {}});
			}

			window.draw(target_line);
			target_line.clear();
		}
	}

	auto Weapon::equip(entt::registry& registry, entt::entity tower_entity, components::EntityType weapon_type) noexcept -> entt::entity
	{
		using namespace components;

		const auto entity = registry.create();

		// 初始为索敌状态
		registry.emplace<tags::weapon_searching>(entity);
		registry.emplace<EntityType>(entity, weapon_type);
		// 指定拥有者
		registry.emplace<weapon::Owner>(entity, tower_entity);

		// todo: 加载配置文件
		{
			std::ignore = weapon_type;

			auto& [search_type, search_strategy, range, fire_rate, fire_timer, on_fire] = registry.emplace<weapon::Specification>(entity);

			search_type = std::to_underlying(enemy::CategoryValue::GROUND) | std::to_underlying(enemy::CategoryValue::AIR);
			search_strategy = weapon::SearchStrategy::DISTANCE_FIRST;
			range = 50.f;
			fire_rate = 1.5f;
			fire_timer = 0.f;
			on_fire =
					[](entt::registry& reg, const entt::entity attacker, const entt::entity victim) noexcept -> void
					{
						// 如果多个武器攻击同一目标,此时该目标可能已经死亡
						// todo: AoE攻击?
						if (not reg.all_of<tags::enemy_alive>(victim))
						{
							// 移除目标
							reg.remove<weapon::Target>(attacker);
							return;
						}

						// 目标不受伤害,什么也不做
						// todo: AoE攻击?
						// todo: 移除目标?
						if (reg.all_of<tags::enemy_immortal>(victim))
						{
							return;
						}

						auto& [health] = reg.get<enemy::Health>(victim);
						health -= 10.f;

						if (health <= 0)
						{
							// 标记为死亡
							reg.remove<tags::enemy_alive>(victim);
							reg.emplace<tags::enemy_killed>(victim);

							// 移除目标
							reg.remove<weapon::Target>(attacker);
						}
					};
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::weapon>(entity);

		// 添加到装备列表
		auto& [weapons] = registry.get<tower::Equipment>(tower_entity);
		weapons.push_back(entity);

		return entity;
	}
}
