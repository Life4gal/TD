#include <systems/weapon.hpp>

#include <components/entity.hpp>
#include <components/weapon.hpp>

#include <helper/observer.hpp>

#include <entt/entt.hpp>

namespace
{
	auto update_cooldown(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		const auto delta_time = delta.asSeconds();

		for (const auto tower_view = registry.view<weapon::Cooldown>();
		     auto [entity, cooldown]: tower_view.each())
		{
			cooldown.delay -= delta_time;

			if (cooldown.delay > 0)
			{
				// 冷却还没好
				continue;
			}

			// 冷却结束
			registry.remove<weapon::Cooldown>(entity);
		}
	}

	auto update_searching(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		for (const auto tower_view = registry.view<const weapon::Range, const entity::Position>(entt::exclude<weapon::Cooldown>);
		     const auto [entity, range, position]: tower_view.each())
		{
			if (const auto target = helper::Observer::find_tower_target(registry, entity, position.position, range.range);
				target != entt::null)
			{
				// 如果能找到一个目标
				// 进入攻击状态
				registry.emplace_or_replace<weapon::Target>(entity, target);
			}
			else
			{
				// 找不到目标, 移除目标组件
				registry.remove<weapon::Target>(entity);
			}
		}
	}

	auto update_fire(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		for (const auto tower_view = registry.view<const weapon::Target, const weapon::FireRate, const weapon::Trigger>(entt::exclude<weapon::Cooldown>);
		     const auto [entity, target, fire_rate, trigger]: tower_view.each())
		{
			// 进入冷却
			registry.emplace<weapon::Cooldown>(entity, fire_rate.fire_rate);

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
