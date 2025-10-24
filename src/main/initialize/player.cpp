#include <initialize/player.hpp>

#include <components/combat/unit.hpp>
#include <components/game/player.hpp>
#include <components/map/map.hpp>

#include <entt/entt.hpp>

namespace initialize
{
	auto player(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		sf::RectangleShape cursor_shape{{static_cast<float>(tile_map.tile_width()), static_cast<float>(tile_map.tile_height())}};

		cursor_shape.setOrigin(cursor_shape.getSize() / 2.f);
		cursor_shape.setFillColor(sf::Color::Transparent);
		cursor_shape.setOutlineColor(sf::Color::Green);
		cursor_shape.setOutlineThickness(2.f);

		registry.ctx().emplace<player::Interaction>(combat::invalid_type);
		registry.ctx().emplace<player::Tower>();
		registry.ctx().emplace<player::Resource>();
		registry.ctx().emplace<player::Statistics>(player::Statistics::size_type{0});
		registry.ctx().emplace<player::Cursor>(std::move(cursor_shape));

		// 初始化玩家资源
		auto& [player_resource] = registry.ctx().get<player::Resource>();

		// todo: 载入配置文件设置资源
		player_resource[resource::Type::HEALTH] = 100;
		player_resource[resource::Type::MANA] = 100;
		player_resource[resource::Type::GOLD] = 100000;
	}
}
