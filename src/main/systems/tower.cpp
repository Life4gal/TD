#include <systems/tower.hpp>

#include <print>

#include <components/tags.hpp>
#include <components/tower.hpp>
#include <components/enemy.hpp>
#include <components/map.hpp>

// #include <components/config.hpp>
// #include <components/sound.hpp>

#include <systems/observer.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

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
						return systems::Observer::query_visible_ground(registry, tower_position, weapon.range);
					}
					else if constexpr (std::is_same_v<Tag, tags::archetype_aerial>)
					{
						return systems::Observer::query_visible_aerial(registry, tower_position, weapon.range);
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
	auto Tower::initialize(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto type = reg.get<const entity::Type>(entity);
				const auto [position] = reg.get<const entity::Position>(entity);

				std::println(
					"[{}] Construct tower {}({}) at ({:.0f}:{:.0f})",
					std::chrono::system_clock::now(),
					std::to_underlying(entity),
					std::to_underlying(type),
					position.x,
					position.y
				);
			}>();

		registry.on_destroy<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println("[{}] Destroy tower {}", std::chrono::system_clock::now(), std::to_underlying(entity));
			}>();

		registry.ctx().emplace<tower::DebugRenderTarget>(sf::VertexArray{sf::PrimitiveType::Lines});
	}

	auto Tower::update(entt::registry& registry, const sf::Time delta) noexcept -> void
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

	auto Tower::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		for (const auto tower_view = registry.view<tower::Render, const entity::Position>(entt::exclude<tags::invisible>);
		     const auto [entity, shape, position]: tower_view.each())
		{
			shape.shape.setPosition(position.position);

			window.draw(shape.shape);
		}

		if (auto* render = registry.ctx().find<tower::DebugRenderTarget>())
		{
			auto& lines = render->lines;

			for (const auto tower_view = registry.view<const tower::Target, tags::tower>();
			     const auto [entity, target]: tower_view.each())
			{
				// 目标可能已经被别的塔击杀
				// if (registry.all_of<tags::dead>(target.entity))
				if (not registry.valid(target.entity))
				{
					continue;
				}

				const auto [tower_position] = registry.get<const entity::Position>(entity);
				const auto [enemy_position] = registry.get<const entity::Position>(target.entity);

				lines.append({.position = tower_position, .color = sf::Color::Red, .texCoords = {}});
				lines.append({.position = enemy_position, .color = sf::Color::Red, .texCoords = {}});
			}

			window.draw(lines);
			lines.clear();
		}
	}

	auto Tower::build(entt::registry& registry, const sf::Vector2u position, const components::entity::Type type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		// 检查资源是否足够以及位置是否合法在外部进行
		assert(tile_map.inside(position.x, position.y) and tile_map.at(position.x, position.y) == map::TileType::TOWER);
		const auto world_position = tile_map.coordinate_grid_to_world(position);

		const auto entity = registry.create();

		registry.emplace<entity::Type>(entity, type);
		registry.emplace<entity::Position>(entity, world_position);

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

			sf::CircleShape shape{};
			{
				shape.setRadius(static_cast<float>(tile_map.tile_width()) * .35f);
				shape.setOrigin({shape.getRadius(), shape.getRadius()});
				shape.setFillColor({200, 150, 50});
			}

			registry.emplace<tower::Render>(entity, std::move(shape));
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::tower>(entity);

		return entity;
	}
}
