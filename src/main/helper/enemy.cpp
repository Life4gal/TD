#include <helper/enemy.hpp>

#include <random>

#include <components/map.hpp>
#include <components/navigation.hpp>

#include <components/tags.hpp>
#include <components/enemy.hpp>

#include <components/sprite_frame.hpp>
#include <components/renderable.hpp>

#include <components/texture.hpp>

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

			// SpriteFrame & Renderable
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

				sprite_frame::Uniform frames
				{
						.frame_duration = sf::seconds(.25f),
						.elapsed_time = sf::Time::Zero,
						.frame_position = {0, 0},
						.frame_size = {16, 16},
						.total_frame_count = 4,
						.current_frame_index = 0,
						.looping = true,
						.playing = true,
				};

				sf::Sprite renderable{texture};
				renderable.setTextureRect({{0, 0}, frames.frame_size});
				renderable.setOrigin(sf::Vector2f{frames.frame_size / 2});

				registry.emplace<sprite_frame::Uniform>(entity, frames);
				registry.emplace<Renderable>(entity, std::move(renderable));
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
}
