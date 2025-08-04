#include <system/game/helper/weapon.hpp>

#include <component/game/tags.hpp>
#include <component/game/enemy.hpp>
#include <component/game/tower.hpp>
#include <component/game/weapon.hpp>

#include <entt/entt.hpp>

namespace
{
	auto do_kill(entt::registry& registry, const entt::entity victim) noexcept -> void
	{
		using namespace game::component;

		// 标记为死亡
		registry.remove<tags::enemy::status::alive>(victim);
		registry.emplace<tags::enemy::status::dead>(victim);
	}
}

namespace game::system::helper
{
	auto Weapon::equip(entt::registry& registry, entt::entity tower_entity, component::entity::Type weapon_type) noexcept -> entt::entity
	{
		using namespace component;

		const auto entity = registry.create();

		// 初始为索敌状态
		registry.emplace<tags::weapon::status::searching>(entity);
		registry.emplace<entity::Type>(entity, weapon_type);
		// 指定拥有者
		registry.emplace<weapon::Owner>(entity, tower_entity);

		// todo: 加载配置文件
		{
			// 可攻击地面
			registry.emplace<tags::weapon::targeting::ground>(entity);
			// 距离优先
			registry.emplace<tags::weapon::strategy::distance>(entity);

			auto& [range, fire_rate, on_fire] = registry.emplace<weapon::Specification>(entity);
			range = 50.f;
			fire_rate = 1.5f;
			on_fire =
					[](entt::registry& reg, const entt::entity attacker, const entt::entity victim) noexcept -> void
					{
						// 如果多个武器攻击同一目标,此时该目标可能已经死亡
						// todo: AoE攻击?
						if (not reg.all_of<tags::enemy::status::alive>(victim))
						{
							// 移除目标
							reg.remove<weapon::Target>(attacker);
							return;
						}

						// 目标不受伤害,什么也不做
						// todo: AoE攻击?
						// todo: 移除目标?
						if (reg.all_of<tags::enemy::condition::immortal>(victim))
						{
							return;
						}

						auto& [health] = reg.get<enemy::Health>(victim);
						health -= 10.f;

						if (health <= 0)
						{
							do_kill(reg, victim);

							// 移除目标
							reg.remove<weapon::Target>(attacker);
						}
					};
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::weapon::identifier>(entity);

		// 添加到装备列表
		auto& [weapons] = registry.get<tower::Equipment>(tower_entity);
		weapons.push_back(entity);

		return entity;
	}
}
