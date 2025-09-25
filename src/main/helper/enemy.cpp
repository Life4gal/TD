#include <helper/enemy.hpp>

#include <random>
#include <print>

#include <components/map.hpp>
#include <components/navigation.hpp>

#include <components/tags.hpp>
#include <components/enemy.hpp>

#include <components/sprite_frame.hpp>
#include <components/renderable.hpp>
#include <components/health_bar.hpp>

#include <components/texture.hpp>

#include <utility/time.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Enemy::spawn(entt::registry& registry, const sf::Vector2u point, const components::entity::Type type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();

		const auto position = tile_map.coordinate_grid_to_world(point);

		const auto entity = registry.create();

		registry.emplace<entity::Type>(entity, type);
		registry.emplace<entity::Position>(entity, position);

		// todo: 加载配置文件
		{
			static std::mt19937 random{std::random_device{}()};

			// 名字
			registry.emplace<entity::Name>(entity, std::format("敌人 0x{:x}", std::to_underlying(type)));

			// 地面单位
			registry.emplace<tags::archetype_ground>(entity);
			const auto direction = flow_field.direction_of(point);
			registry.emplace<enemy::Direction>(entity, direction);

			// 生命值
			auto& [health] = registry.emplace<enemy::Health>(entity);
			health = 100;

			// 移动速度
			auto& [speed] = registry.emplace<enemy::Movement>(entity);
			speed = 30.f + std::uniform_real_distribution<float>{0, 20}(random);

			// 强度
			auto& [power] = registry.emplace<enemy::Power>(entity);
			power = 1 + std::uniform_int_distribution<enemy::Power::value_type>{0, 100}(random);

			// SpriteFrame & Renderable & HealthBar
			{
				const auto& texture = [&]() -> decltype(auto)
				{
					auto& [textures] = registry.ctx().get<Textures>();

					constexpr std::string_view enemy_name{"deep-dive-AntleredRascal"};
					constexpr entt::basic_hashed_string enemy_hash_name{enemy_name.data(), enemy_name.size()};

					auto [it, result] = textures.load(enemy_hash_name, loaders::TextureType::ENEMY, enemy_name);
					assert(it->second);

					return *it->second;
				}();

				constexpr auto frame_size = sf::Vector2i{16, 16};

				// SpriteFrame
				registry.emplace<sprite_frame::Timer>(
					entity,
					sprite_frame::Timer{.frame_duration = sf::seconds(.25f), .elapsed_time = sf::Time::Zero}
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

				// Renderable
				sf::Sprite renderable{texture};
				renderable.setTextureRect({{0, 0}, frame_size});
				renderable.setOrigin(sf::Vector2f{frame_size / 2});
				registry.emplace<Renderable>(entity, std::move(renderable));

				// HealthBar
				registry.emplace<health_bar::Health>(entity, health);
				registry.emplace<health_bar::Size>(entity, sf::Vector2f{static_cast<float>(frame_size.x), static_cast<float>(frame_size.y / 4)}); // NOLINT(bugprone-integer-division)
				registry.emplace<health_bar::Offset>(entity, sf::Vector2f{static_cast<float>(-frame_size.x / 2), -static_cast<float>(frame_size.y / 2) - 5.f}); // NOLINT(bugprone-integer-division)
			}

			// 图集纹理大小为16*16,放大一些
			registry.emplace<entity::Scale>(entity, sf::Vector2f{2.f, 2.f});
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::enemy>(entity);

		return entity;
	}

	auto Enemy::spawn(entt::registry& registry, const std::uint32_t start_gate_id, const components::entity::Type type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();
		const auto start_gate = start_gates[start_gate_id];

		return spawn(registry, start_gate, type);
	}

	auto Enemy::reach(entt::registry& registry, const entt::entity enemy) noexcept -> void
	{
		using namespace components;

		assert(registry.all_of<tags::enemy>(enemy));

		registry.emplace<tags::dead>(enemy);
		registry.emplace<tags::cod_reached>(enemy);
	}

	auto Enemy::kill(entt::registry& registry, const entt::entity attacker, const entt::entity victim) noexcept -> void
	{
		using namespace components;

		assert(registry.valid(attacker));
		assert(registry.valid(victim));
		assert(registry.all_of<tags::enemy>(victim));

		const auto& [attacker_name] = registry.get<const entity::Name>(attacker);
		const auto& [victim_name] = registry.get<const entity::Name>(victim);

		// 如果目标已经死亡则什么也不做
		if (registry.all_of<tags::dead>(victim))
		{
			std::println(
				"[{:%Y-%m-%d %H:%M:%S}] [{}](EID:{})试图击杀[{}](EID:{}),但是其已死亡",
				utility::zoned_now(),
				attacker_name,
				std::to_underlying(attacker),
				victim_name,
				std::to_underlying(victim)
			);
			return;
		}

		std::println(
			"[{:%Y-%m-%d %H:%M:%S}] [{}](EID:{})击杀[{}](EID:{})",
			utility::zoned_now(),
			attacker_name,
			std::to_underlying(attacker),
			victim_name,
			std::to_underlying(victim)
		);

		registry.emplace<tags::dead>(victim);
		registry.emplace<tags::cod_killed>(victim);
	}

	auto Enemy::hurt(entt::registry& registry, const entt::entity attacker, const entt::entity victim, const float damage) noexcept -> void
	{
		using namespace components;

		assert(registry.valid(attacker));
		assert(registry.valid(victim));
		assert(registry.all_of<tags::enemy>(victim));

		const auto& [attacker_name] = registry.get<const entity::Name>(attacker);
		const auto& [victim_name] = registry.get<const entity::Name>(victim);

		// 如果目标已经死亡则什么也不做
		if (registry.all_of<tags::dead>(victim))
		{
			std::println(
				"[{:%Y-%m-%d %H:%M:%S}] [{}](EID:{})试图伤害[{}](EID:{}),但是其已死亡",
				utility::zoned_now(),
				attacker_name,
				std::to_underlying(attacker),
				victim_name,
				std::to_underlying(victim)
			);
			return;
		}

		auto& [health] = registry.get<enemy::Health>(victim);

		const auto new_health = health - damage;
		const auto old_health = std::exchange(health, new_health);

		std::println(
			"[{:%Y-%m-%d %H:%M:%S}] [{}](EID:{})对[{}](EID:{})造成了{:.3f}点伤害({:.3f} ==> {:.3f})",
			utility::zoned_now(),
			attacker_name,
			std::to_underlying(attacker),
			victim_name,
			std::to_underlying(victim),
			damage,
			old_health,
			new_health
		);

		health -= damage;
		if (health <= 0)
		{
			kill(registry, attacker, victim);
		}
	}
}
