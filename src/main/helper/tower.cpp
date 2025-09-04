#include <helper/tower.hpp>

#include <components/map.hpp>

#include <components/tags.hpp>
#include <components/tower.hpp>
#include <components/enemy.hpp>

// #include <components/config.hpp>
// #include <components/sound.hpp>
// #include <components/texture.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Tower::build(entt::registry& registry, components::entity::Type type) noexcept -> entt::entity
	{
		using namespace components;

		const auto entity = registry.create();

		registry.emplace<entity::Type>(entity, type);
		// 位置由外部设置,这里只负责构建实体
		// registry.emplace<entity::Position>(entity, position);

		// todo: 读取配置文件
		{
			// auto& [configs] = registry.ctx().get<Configs>();

			// 可攻击地面
			registry.emplace<tags::targeting_ground>(entity);
			// 距离优先
			registry.emplace<tags::strategy_distance_first>(entity);
			// 武器
			auto& [range, fire_rate, on_fire] = registry.emplace<tower::Weapon>(entity);
			range = 50.f;
			fire_rate = 1.5f;
			on_fire =
					[](entt::registry& reg, [[maybe_unused]] const entt::entity attacker, const entt::entity victim) noexcept -> void
					{
						// 如果多个武器攻击同一目标,此时该目标可能已经死亡
						// todo: AoE攻击?
						if (reg.all_of<tags::dead>(victim))
						{
							// 移除目标
							reg.remove<tower::Target>(attacker);
							return;
						}

						// 攻击效果
						{
							// const auto& [from] = reg.get<const entity::Position>(attacker);
							// const auto& [to] = reg.get<const entity::Position>(victim);
							// Bullet::laser(reg, from, to, sf::Color::Green);
						}

						// 攻击音效
						{
							// auto& [sounds] = reg.ctx().get<Sounds>();
							//
							// const auto& [shooting_sound] = reg.get<const sound::Shooting>(attacker);
							// sounds.play(shooting_sound);
						}

						// todo: 不应该在这里负责对敌人造成伤害
						auto& [health] = reg.get<enemy::Health>(victim);
						health -= 10.f;

						if (health <= 0)
						{
							reg.emplace<tags::dead>(victim);
							reg.emplace<tags::enemy_killed>(victim);

							// 移除目标
							reg.remove<tower::Target>(attacker);
						}
					};

			// render
			{
				const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

				sf::CircleShape shape{};
				{
					shape.setRadius(static_cast<float>(tile_map.tile_width()) * .35f);
					shape.setOrigin({shape.getRadius(), shape.getRadius()});
					shape.setFillColor({200, 150, 50});
				}

				registry.emplace<tower::Render>(entity, std::move(shape));
			}
		}

		return entity;
	}
}
