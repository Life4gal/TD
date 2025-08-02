#include <systems/helper/weapon.hpp>

#include <components/tags.hpp>
#include <components/enemy.hpp>
#include <components/tower.hpp>
#include <components/weapon.hpp>

#include <entt/entt.hpp>

namespace systems::helper
{
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
