#include <systems/initialize.hpp>

#include <print>

#include <components/map.hpp>
#include <components/wave.hpp>
#include <components/navigation.hpp>
#include <components/observer.hpp>

#include <components/player.hpp>
#include <components/resource.hpp>
#include <components/hud.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>

#include <components/debug.hpp>

#include <components/font.hpp>

#include <helper/wave.hpp>

#include <utility/time.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace
{
	auto do_initialize_map(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		std::ignore = registry;
	}

	auto do_initialize_wave(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::wave>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto [wave_index] = reg.get<const wave::WaveIndex>(entity);
				const auto& [spawns] = reg.get<const wave::Wave>(entity);

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 生成波次{},共{}个敌人(EID:{})",
					utility::zoned_now(),
					wave_index,
					spawns.size(),
					std::to_underlying(entity)
				);
			}>();

		registry.on_destroy<tags::wave>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 结束波次(EID:{})",
					utility::zoned_now(),
					std::to_underlying(entity)
				);
			}>();
	}

	auto do_initialize_navigation(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();
		const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();

		map::FlowField flow_field{tile_map};
		{
			flow_field.build(end_gates);
		}

		std::vector<map::path_type> cache_paths{};
		{
			cache_paths.reserve(start_gates.size());

			// 为每个起点到最佳终点计算路径
			for (const auto start: start_gates)
			{
				auto path = flow_field.path_of(start, std::numeric_limits<std::size_t>::max());
				assert(path.has_value());

				cache_paths.emplace_back(std::move(*path));
			}
		}

		registry.ctx().emplace<navigation::FlowField>(std::move(flow_field));
		registry.ctx().emplace<navigation::Path>(std::move(cache_paths));
	}

	auto do_initialize_observer(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		using size_type = observer::EnemyStatistics::size_type;

		registry.ctx().emplace<observer::EnemyArchetype>();
		registry.ctx().emplace<observer::EnemyStatistics>(size_type{0}, size_type{0});
	}

	auto do_initialize_player(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		sf::RectangleShape cursor_shape{{static_cast<float>(tile_map.tile_width()), static_cast<float>(tile_map.tile_height())}};

		cursor_shape.setOrigin(cursor_shape.getSize() / 2.f);
		cursor_shape.setFillColor(sf::Color::Transparent);
		cursor_shape.setOutlineColor(sf::Color::Green);
		cursor_shape.setOutlineThickness(2.f);

		registry.ctx().emplace<player::Interaction>(entity::invalid_type);
		registry.ctx().emplace<player::Tower>();
		registry.ctx().emplace<player::Resource>();
		registry.ctx().emplace<player::Statistics>(player::Statistics::size_type{0});
		registry.ctx().emplace<player::Cursor>(std::move(cursor_shape));
	}

	auto do_initialize_resource(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [player_resource] = registry.ctx().get<player::Resource>();

		// todo: 载入配置文件设置资源
		player_resource[resource::Type::HEALTH] = 100;
		player_resource[resource::Type::MANA] = 100;
		player_resource[resource::Type::GOLD] = 100000;
	}

	auto do_initialize_hud(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [fonts] = registry.ctx().get<Fonts>();

		const auto [it, result] = fonts.load(constants::hud, "hud");
		assert(result);
		assert(it->second);

		registry.ctx().emplace<hud::Text>(sf::Text{it->second, "", 25});
	}

	auto do_initialize_tower(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto type = reg.get<const entity::Type>(entity);
				const auto [position] = reg.get<const entity::Position>(entity);
				const auto& [name] = reg.get<const entity::Name>(entity);

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 在({:.0f}:{:.0f})建造[0x{:08x}]型塔[{}](EID:{})",
					utility::zoned_now(),
					position.x,
					position.y,
					std::to_underlying(type),
					name,
					std::to_underlying(entity)
				);
			}>();

		registry.on_destroy<tags::tower>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 销毁塔(EID:{})",
					utility::zoned_now(),
					std::to_underlying(entity)
				);
			}>();
	}

	auto do_initialize_enemy(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.on_construct<tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				const auto type = reg.get<const entity::Type>(entity);
				const auto [position] = reg.get<const entity::Position>(entity);
				const auto& [name] = reg.get<const entity::Name>(entity);

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 在({:.0f}:{:.0f})生成[0x{:08x}]型敌人[{}](EID:{})",
					utility::zoned_now(),
					position.x,
					position.y,
					std::to_underlying(type),
					name,
					std::to_underlying(entity)
				);
			}>();

		registry.on_destroy<tags::enemy>().connect<
			[](const entt::registry& reg, const entt::entity entity) noexcept -> void
			{
				std::ignore = reg;

				std::println(
					"[{:%Y-%m-%d %H:%M:%S}] 销毁敌人(EID:{})",
					utility::zoned_now(),
					std::to_underlying(entity)
				);
			}>();
	}

	auto do_initialize_debug(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.ctx().emplace<DebugRenderPath>(sf::VertexArray{sf::PrimitiveType::LineStrip});
		registry.ctx().emplace<DebugRenderFlow>(sf::VertexArray{sf::PrimitiveType::Lines});
		registry.ctx().emplace<DebugRenderTarget>(sf::VertexArray{sf::PrimitiveType::Lines});
	}
}

namespace systems
{
	auto Initialize::initialize(entt::registry& registry) noexcept -> void
	{
		// 初始化地图
		do_initialize_map(registry);
		// 初始化波次
		do_initialize_wave(registry);
		// 初始化导航
		do_initialize_navigation(registry);
		// 初始化观察者
		do_initialize_observer(registry);

		// 初始化玩家
		do_initialize_player(registry);
		// 初始化玩家资源
		do_initialize_resource(registry);
		// 初始化玩家HUD
		do_initialize_hud(registry);

		// 初始化塔
		do_initialize_tower(registry);
		// 初始化敌人
		do_initialize_enemy(registry);

		// 初始化debug
		do_initialize_debug(registry);
	}
}
