#include <helper/tower.hpp>

#include <components/tags.hpp>
#include <components/tower.hpp>

#include <components/sprite_frame.hpp>
#include <components/renderable.hpp>

#include <helper/asset.hpp>
#include <helper/enemy.hpp>

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

			// 名字
			registry.emplace<entity::Name>(entity, std::format("塔 0x{:x}", std::to_underlying(type)));

			// 可攻击地面
			registry.emplace<tags::targeting_ground>(entity);
			// 距离优先
			registry.emplace<tags::strategy_distance_first>(entity);
			// 武器
			auto& [range, fire_rate] = registry.emplace<tower::Weapon>(entity);
			range = 50.f;
			fire_rate = 1.5f;
			// 开火
			auto& [on_fire] = registry.emplace<tower::Trigger>(entity);
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
						Enemy::hurt(reg, attacker, victim, 10.f);
					};

			// SpriteFrame & Renderable
			{
				// todo: 还没有为塔准备纹理,这里暂时使用敌人的纹理
				constexpr std::string_view tower_name{"deep-dive-WarpSkull"};
				constexpr entt::basic_hashed_string tower_hash_name{tower_name.data(), tower_name.size()};
				const auto& texture = Asset::texture_of(registry, tower_hash_name);

				constexpr auto frame_size = sf::Vector2i{16, 16};

				sf::Sprite renderable{texture};
				renderable.setTextureRect({{0, 0}, frame_size});
				renderable.setOrigin(sf::Vector2f{frame_size / 2});

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

				registry.emplace<Renderable>(entity, std::move(renderable));
			}

			// 图集纹理大小为16*16,放大一些
			registry.emplace<entity::Scale>(entity, sf::Vector2f{2.5f, 2.5f});
		}

		return entity;
	}
}
