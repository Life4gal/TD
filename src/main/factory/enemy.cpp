#include <factory/enemy.hpp>

#include <random>

#include <components/core/tags.hpp>
#include <components/core/transform.hpp>
#include <components/core/renderable.hpp>
#include <components/core/sprite_frame.hpp>
#include <components/combat/enemy.hpp>
#include <components/combat/health_bar.hpp>
#include <components/map/map.hpp>
#include <components/map/navigation.hpp>

#include <helper/asset.hpp>

#include <entt/entt.hpp>

namespace factory
{
	auto enemy(entt::registry& registry, const sf::Vector2u point, const components::combat::Type type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();
		const auto& [flow_field] = registry.ctx().get<const navigation::FlowField>();

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
			registry.emplace<transform::Scale>(entity, sf::Vector2f{2.f, 2.f});
			registry.emplace<transform::Rotation>(entity, sf::degrees(0));
		}
		// renderable & sprite_frame
		{
			constexpr std::string_view enemy_name{"deep-dive-AntleredRascal"};
			constexpr entt::basic_hashed_string enemy_hash_name{enemy_name.data(), enemy_name.size()};
			const auto& texture = helper::Asset::texture_of(registry, enemy_hash_name);

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
		}

		// ======================
		// COMBAT
		// ======================

		// unit
		{
			registry.emplace<combat::Type>(entity, type);
			registry.emplace<combat::Name>(entity, std::format("AntleredRascal #{}", entt::to_integral(entity)));
			// registry.emplace<combat::OnDeath>(entity, nullptr);
		}
		// enemy & health_bar
		{
			static std::mt19937 random{std::random_device{}()};

			// 生命值
			auto& [health] = registry.emplace<enemy::Health>(entity);
			health = 100;

			// 移动速度
			auto& [speed] = registry.emplace<enemy::Movement>(entity);
			speed = 30.f + std::uniform_real_distribution<float>{0, 20}(random);

			// 强度
			auto& [power] = registry.emplace<enemy::Power>(entity);
			power = 1 + std::uniform_int_distribution<enemy::Power::value_type>{0, 100}(random);

			// 地面单位
			registry.emplace<tags::archetype_ground>(entity);
			const auto direction = flow_field.direction_of(point);
			registry.emplace<enemy::Direction>(entity, direction);

			// HealthBar
			{
				const auto& [sprite] = registry.get<const Renderable>(entity);
				const auto frame_size = sprite.getTextureRect().size;

				registry.emplace<health_bar::Health>(entity, health);
				registry.emplace<health_bar::Size>(entity, sf::Vector2f{static_cast<float>(frame_size.x), static_cast<float>(frame_size.y / 4)}); // NOLINT(bugprone-integer-division)
				registry.emplace<health_bar::Offset>(entity, sf::Vector2f{static_cast<float>(-frame_size.x / 2), -static_cast<float>(frame_size.y / 2) - 5.f}); // NOLINT(bugprone-integer-division)
			}
		}

		// 初始化完成后才注册该标记,如此方便获取设置的实体信息
		registry.emplace<tags::enemy>(entity);

		return entity;
	}

	auto enemy(entt::registry& registry, const std::uint32_t start_gate_id, const components::combat::Type type) noexcept -> entt::entity
	{
		using namespace components;

		const auto& [start_gates] = registry.ctx().get<const map_ex::StartGate>();
		const auto start_gate = start_gates[start_gate_id];

		return enemy(registry, start_gate, type);
	}
}
