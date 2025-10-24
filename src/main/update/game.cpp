#include <update/game.hpp>

#include <components/game.hpp>

#include <entt/entt.hpp>

namespace update
{
	auto game(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		auto& [frame_delta] = registry.ctx().get<game::FrameDelta>();
		auto& [elapsed] = registry.ctx().get<game::ElapsedTime>();

		frame_delta = delta;
		elapsed += delta;
	}

	auto game_simulation(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		auto& [elapsed] = registry.ctx().get<game::ElapsedSimulationTime>();

		elapsed += delta;
	}
}
