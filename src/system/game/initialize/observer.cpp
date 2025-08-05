#include <system/game/initialize/observer.hpp>

#include <component/game/observer.hpp>

#include <entt/entt.hpp>

namespace game::system::initialize
{
	auto observer(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		using size_type = observer::EnemyStatistics::size_type;

		registry.ctx().emplace<observer::EnemyArchetype>();
		registry.ctx().emplace<observer::EnemyStatistics>(size_type{0});
	}
}
