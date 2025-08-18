#pragma once

#include <scene/scene.hpp>

namespace scene
{
	class Game final : public Scene
	{
	public:
		Game(const Game&) noexcept = delete;
		Game(Game&&) noexcept = delete;
		auto operator=(const Game&) noexcept -> Game& = delete;
		auto operator=(Game&&) noexcept -> Game& = delete;

	private:
		std::uint32_t simulation_times_per_tick_;

		auto do_update_simulation(sf::Time delta) noexcept -> void;
		auto do_update() noexcept -> void;

	public:
		~Game() noexcept override;

		explicit Game(std::shared_ptr<entt::registry> global_registry) noexcept;

		auto handle_event(const sf::Event& event) noexcept -> void override;

		auto update(sf::Time delta) noexcept -> void override;

		auto render(sf::RenderWindow& window) noexcept -> void override;
	};
}
