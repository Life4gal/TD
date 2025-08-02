#include <systems/initialize/resource.hpp>

#include <components/player.hpp>

#include <entt/entt.hpp>

namespace systems::initialize
{
	auto resource(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [player_resources] = registry.ctx().get<player::Resource>();

		// todo: 加载配置文件
		player_resources[resource::Type::HEALTH] = 100;
		player_resources[resource::Type::MANA] = 100;
		player_resources[resource::Type::GOLD] = 9999999;
	}
}
