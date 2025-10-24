#include <initialize/observer.hpp>

#include <components/map/observer.hpp>

#include <entt/entt.hpp>

namespace initialize
{
	auto observer(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.ctx().emplace<observer::GroundEnemy>();
		registry.ctx().emplace<observer::AerialEnemy>();
	}
}
