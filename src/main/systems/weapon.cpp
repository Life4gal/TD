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

		for (const auto tower_view = registry.view<tower::Weapon, const entity::Position>(entt::exclude<tower::Cooldown>);
		     const auto [entity, specifications, position]: tower_view.each())
		{
			const auto [tower_position] = registry.get<const entity::Position>(entity);

			const auto target_enemy = [&]() noexcept -> entt::entity
			{
				// 瞄准目标必须可见
				const auto enemies_in_range = [&]
				{
					using enemy::Archetype;

					const auto do_query = [&](const Archetype archetype) noexcept -> std::vector<entt::entity>
					{
						return helper::Observer::query(registry, archetype, true, position.position, specifications.range);
					};

					const auto targeting_ground = registry.all_of<tags::targeting_ground>(entity);
					const auto targeting_air = registry.all_of<tags::targeting_air>(entity);

					// 可同时对地/对空
					if (targeting_ground and targeting_air)
					{
						if (registry.all_of<tags::strategy_ground_first>(entity))
						{
							if (auto r = do_query(Archetype::GROUND);
								not r.empty())
							{
								return r;
							}

							return do_query(Archetype::AERIAL);
						}

						if (registry.all_of<tags::strategy_air_first>(entity))
						{
							if (auto r = do_query(Archetype::AERIAL);
								not r.empty())
							{
								return r;
							}

							return do_query(Archetype::GROUND);
						}

						return do_query(Archetype::DUAL);
					}

					// 对地
					if (targeting_ground)
					{
						return do_query(Archetype::GROUND);
					}

					// 对空
					if (targeting_air)
					{
						return do_query(Archetype::AERIAL);
					}

					std::unreachable();
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
		}
	}

	auto update_fire(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		for (const auto weapon_view = registry.view<const tower::Target, const tower::Weapon, const tower::Trigger>(entt::exclude<tower::Cooldown>);
		     const auto [entity, target, weapon, trigger]: weapon_view.each())
		{
			// 进入冷却
			registry.emplace<tower::Cooldown>(entity, weapon.fire_rate);

			// 触发攻击
			trigger.on_fire(registry, entity, target.entity);
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
