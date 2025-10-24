#include <initialize/game.hpp>

#include <components/game/game.hpp>

#include <entt/entt.hpp>

namespace initialize
{
	auto game(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		registry.ctx().emplace<game::FrameDelta>(sf::seconds(1));
		registry.ctx().emplace<game::ElapsedTime>(sf::Time::Zero);
		registry.ctx().emplace<game::ElapsedSimulationTime>(sf::Time::Zero);
	}
}
