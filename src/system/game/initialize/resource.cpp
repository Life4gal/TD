#include <system/game/initialize/resource.hpp>

#include <component/game/player.hpp>

#include <entt/entt.hpp>

namespace game::system::initialize
{
	auto resource(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		auto& [player_resource] = registry.ctx().get<player::Resource>();

		// todo: 加载配置文件
		player_resource[resource::Type::HEALTH] = 100;
		player_resource[resource::Type::MANA] = 100;
		player_resource[resource::Type::GOLD] = 9999999;
	}
}
