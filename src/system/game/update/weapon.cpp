#include <system/game/update/weapon.hpp>

#include <component/game/tags.hpp>
#include <component/game/entity.hpp>
#include <component/game/enemy.hpp>
#include <component/game/weapon.hpp>

#include <system/game/helper/observer.hpp>

#include <entt/entt.hpp>

namespace
{
	using namespace game;

	auto update_cooldown(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace component;

		const auto delta_time = delta.asSeconds();

		for (const auto weapon_view = registry.view<tags::weapon::identifier, const weapon::Specification, weapon::Cooldown>();
		     auto [entity, weapon, cooldown]: weapon_view.each())
		{
			cooldown.delay -= delta_time;

			if (cooldown.delay > 0)
			{
				// 冷却还没好
				continue;
			}

			// 冷却结束
			registry.remove<weapon::Cooldown>(entity);
			registry.emplace<tags::weapon::status::searching>(entity);
		}
	}

	auto update_searching(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		const auto do_select_target = [&]<typename Tag>(const entt::entity entity, const weapon::Specification& weapon, const entt::entity owner) noexcept -> void
		{
			assert(registry.valid(owner));

			const auto [owner_position] = registry.get<const entity::Position>(owner);

			const auto target_enemy = [&]() noexcept -> entt::entity
			{
				const auto enemies_in_range = [&]
				{
					using namespace system;

					if constexpr (std::is_same_v<Tag, tags::enemy::archetype::ground>)
					{
						return helper::Observer::query_ground(registry, owner_position, weapon.range);
					}
					else if constexpr (std::is_same_v<Tag, tags::enemy::archetype::aerial>)
					{
						return helper::Observer::query_aerial(registry, owner_position, weapon.range);
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
				if (registry.all_of<tags::weapon::strategy::distance>(entity))
				{
					const auto min_distance_enemy = std::ranges::min(
						enemies_in_range,
						std::ranges::less{},
						[&](const entt::entity enemy) noexcept -> float
						{
							const auto enemy_position = registry.get<const entity::Position>(enemy).position;

							const auto dx = owner_position.x - enemy_position.x;
							const auto dy = owner_position.y - enemy_position.y;

							return dx * dx + dy * dy;
						}
					);

					return min_distance_enemy;
				}

				// 强度优先
				if (registry.all_of<tags::weapon::strategy::power>(entity))
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
				registry.emplace_or_replace<weapon::Target>(entity, target_enemy);

				// 进入攻击状态
				registry.remove<tags::weapon::status::searching>(entity);
				registry.emplace<tags::weapon::status::attacking>(entity);
			}
			else
			{
				// 找不到目标, 保持索敌状态

				// 移除目标组件
				registry.remove<weapon::Target>(entity);
			}
		};

		// 优先处理对空
		for (const auto weapon_view = registry.view<tags::weapon::identifier, tags::weapon::status::searching, tags::weapon::targeting::air, const weapon::Specification, const weapon::Owner>();
		     const auto [entity, weapon, owner]: weapon_view.each())
		{
			do_select_target.operator()<tags::enemy::archetype::aerial>(entity, weapon, owner.entity);
		}
		// 再处理对地(也包括没找到空中目标的可对地武器)
		for (const auto weapon_view = registry.view<tags::weapon::identifier, tags::weapon::status::searching, tags::weapon::targeting::ground, const weapon::Specification, const weapon::Owner>();
		     const auto [entity, weapon, owner]: weapon_view.each())
		{
			do_select_target.operator()<tags::enemy::archetype::ground>(entity, weapon, owner.entity);
		}
	}

	auto update_fire(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		for (const auto weapon_view = registry.view<tags::weapon::identifier, tags::weapon::status::attacking, const weapon::Specification, const weapon::Target>();
		     const auto [entity, weapon, target]: weapon_view.each())
		{
			registry.remove<tags::weapon::status::attacking>(entity);
			registry.emplace<weapon::Cooldown>(entity, weapon.fire_rate);

			weapon.on_fire(registry, entity, target.entity);
		}
	}
}

namespace game::system::update
{
	auto weapon(entt::registry& registry, const sf::Time delta) noexcept -> void
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
}
