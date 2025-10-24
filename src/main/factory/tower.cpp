#include <factory/tower.hpp>

#include <random>

#include <components/core/tags.hpp>
#include <components/core/transform.hpp>
#include <components/core/renderable.hpp>
#include <components/core/sprite_frame.hpp>
#include <components/combat/tower.hpp>
#include <components/combat/weapon.hpp>
#include <components/map/map.hpp>

#include <helper/enemy.hpp>

#include <entt/entt.hpp>

namespace factory
{
	auto tower(entt::registry& registry, const sf::Vector2u point, const components::combat::Type type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		const auto position = tile_map.coordinate_grid_to_world(point);

		const auto entity = registry.create();

		// todo: 加载配置文件

		// ======================
		// CORE
		// ======================

		// transform
		{
			registry.emplace<transform::Position>(entity, position);
			// 图集纹理大小为16*16,放大一些
			registry.emplace<transform::Scale>(entity, sf::Vector2f{2.5f, 2.5f});
			registry.emplace<transform::Rotation>(entity, sf::degrees(0));
		}
		// renderable & sprite_frame
		{
			constexpr auto frame_size = sf::Vector2i{16, 16};

			// todo: 还没有为塔准备纹理,这里暂时使用敌人的纹理
			constexpr std::string_view tower_name{"deep-dive-WarpSkull"};
			constexpr entt::basic_hashed_string tower_hash_name{tower_name.data(), tower_name.size()};

			// const auto& texture = helper::Asset::texture_of(registry, tower_hash_name);
			registry.emplace<renderable::Texture>(entity, tower_hash_name);
			registry.emplace<renderable::Area>(entity, sf::IntRect{{0, 0}, frame_size});
			registry.emplace<renderable::Origin>(entity, sf::Vector2f{frame_size / 2});
			registry.emplace<renderable::Color>(entity, sf::Color::White);

			registry.emplace<sprite_frame::Timer>(
				entity,
				sprite_frame::Timer{.frame_duration = sf::seconds(.5f), .elapsed_time = sf::Time::Zero}
			);
			registry.emplace<sprite_frame::Condition>(
				entity,
				sprite_frame::Condition{.looping = true, .playing = true}
			);
			registry.emplace<sprite_frame::Frame>(
				entity,
				sprite_frame::Frame{.total_count = 4, .current_index = 0}
			);
			registry.emplace<sprite_frame::Uniform>(
				entity,
				sprite_frame::Uniform{.frame_position = {0, 0}, .frame_size = frame_size}
			);
		}

		// ======================
		// COMBAT
		// ======================

		// unit
		{
			registry.emplace<combat::Type>(entity, type);
			registry.emplace<combat::Name>(entity, std::format("WarpSkull #{}", entt::to_integral(entity)));
		}
		// tower & weapon
		{
			// 武器配置
			registry.emplace<weapon::Range>(entity, 50.f);
			registry.emplace<weapon::FireRate>(entity, 1.5f);

			// 开火
			registry.emplace<weapon::Trigger>(
				entity,
				[](entt::registry& reg, const entt::entity attacker, const entt::entity victim) noexcept -> void
				{
					// 如果多个武器攻击同一目标,此时该目标可能已经死亡
					// todo: AoE攻击?
					if (reg.all_of<tags::dead>(victim))
					{
						// 移除目标
						reg.remove<weapon::Target>(attacker);
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
					helper::Enemy::hurt(reg, attacker, victim, 10.f);
				}
			);

			// 初始处于冷却状态
			registry.emplace<weapon::Cooldown>(entity, weapon::Cooldown{.delay = .001f});
			// 初始没有目标
			// registry.emplace<weapon::Target>(entity, entt::null);

			// 攻击地面
			registry.emplace<tags::targeting_ground>(entity);
			// 无优先
			// registry.emplace<tags::strategy_ground_first>(entity);
			// 距离优先
			registry.emplace<tags::strategy_distance_first>(entity);
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::tower>(entity);

		return entity;
	}
}
