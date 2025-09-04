#include <systems/weapon.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/tower.hpp>
#include <components/enemy.hpp>

#include <helper/observer.hpp>

#include <entt/entt.hpp>

namespace
{
	auto update_cooldown(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		const auto delta_time = delta.asSeconds();

		for (const auto weapon_view = registry.view<tower::Cooldown, tower::Weapon>();
		     auto [entity, cooldown, weapon]: weapon_view.each())
		{
			cooldown.delay -= delta_time;

			if (cooldown.delay > 0)
			{
				// 冷却还没好
				continue;
			}

			// 冷却结束
			registry.remove<tower::Cooldown>(entity);
		}
	}

	auto update_searching(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto do_select_target = [&]<typename Tag>(const entt::entity entity, const tower::Weapon& weapon) noexcept -> void
		{
			assert(registry.valid(entity));

			const auto [tower_position] = registry.get<const entity::Position>(entity);

			const auto target_enemy = [&]() noexcept -> entt::entity
			{
				const auto enemies_in_range = [&]
				{
					// 瞄准目标必须可见
					if constexpr (std::is_same_v<Tag, tags::archetype_ground>)
					{
						return helper::Observer::query_visible_ground(registry, tower_position, weapon.range);
					}
					else if constexpr (std::is_same_v<Tag, tags::archetype_aerial>)
					{
						return helper::Observer::query_visible_aerial(registry, tower_position, weapon.range);
					}
					else
					{
						std::unreachable();
					}
				}();

				if (enemies_in_range.empty())
				{
					return entt::null;
				}

				// 距离优先
				if (registry.all_of<tags::strategy_distance_first>(entity))
				{
					const auto min_distance_enemy = std::ranges::min(
						enemies_in_range,
						std::ranges::less{},
						[&](const entt::entity enemy) noexcept -> float
						{
							const auto enemy_position = registry.get<const entity::Position>(enemy).position;

							const auto dx = tower_position.x - enemy_position.x;
							const auto dy = tower_position.y - enemy_position.y;

							return dx * dx + dy * dy;
						}
					);

					return min_distance_enemy;
				}

				// 强度优先
				if (registry.all_of<tags::strategy_power_first>(entity))
				{
					const auto max_power_enemy = std::ranges::max(
						enemies_in_range,
						std::ranges::greater{},
						[&](const entt::entity enemy) noexcept -> enemy::Power::value_type
						{
							return registry.get<const enemy::Power>(enemy).power;
						}
					);

					return max_power_enemy;
				}

				std::unreachable();
			}();

			// 如果能找到一个目标
			if (target_enemy != entt::null)
			{
				// 进入攻击状态
				registry.emplace_or_replace<tower::Target>(entity, target_enemy);
			}
			else
			{
				// 找不到目标, 移除目标组件
				registry.remove<tower::Target>(entity);
			}
		};

		// 优先处理对空
		for (const auto weapon_view = registry.view<tags::targeting_air, tower::Weapon>(entt::exclude<tower::Cooldown>);
		     const auto [entity, weapon]: weapon_view.each())
		{
			do_select_target.operator()<tags::archetype_aerial>(entity, weapon);
		}
		// 再处理对地(也包括没找到空中目标的可对地武器)
		for (const auto weapon_view = registry.view<tags::targeting_ground, tower::Weapon>(entt::exclude<tower::Cooldown>);
		     const auto [entity, weapon]: weapon_view.each())
		{
			do_select_target.operator()<tags::archetype_ground>(entity, weapon);
		}
	}

	auto update_fire(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		for (const auto weapon_view = registry.view<const tower::Target, const tower::Weapon>(entt::exclude<tower::Cooldown>);
		     const auto [entity, target, weapon]: weapon_view.each())
		{
			// 进入冷却
			registry.emplace<tower::Cooldown>(entity, weapon.fire_rate);

			// 触发攻击
			weapon.on_fire(registry, entity, target.entity);
		}
	}
}

namespace systems
{
	auto Weapon::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		// ===================================================
		// 1. 如果武器处于攻击冷却中,检测是否可以攻击

		update_cooldown(registry, delta);

		// ===================================================
		// 2. 如果武器处于索敌状态,寻找目标

		update_searching(registry);

		// ===================================================
		// 3. 如果武器处于攻击状态,进行攻击

		update_fire(registry);
	}
}
