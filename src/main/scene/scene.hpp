#pragma once

#include <memory>

#include <entt/entity/registry.hpp>

#include <SFML/System/Time.hpp>

namespace sf
{
	class Event;
	class RenderWindow;
}

namespace scene
{
	class Scene
	{
	public:
		Scene(const Scene&) noexcept = delete;
		Scene(Scene&&) noexcept = delete;
		auto operator=(const Scene&) noexcept -> Scene& = delete;
		auto operator=(Scene&&) noexcept -> Scene& = delete;

	protected:
		entt::registry scene_registry_;

	public:
		explicit Scene(std::shared_ptr<entt::registry> global_registry) noexcept;

		virtual ~Scene() noexcept;

		virtual auto handle_event(const sf::Event& event) noexcept -> void = 0;

		virtual auto update(sf::Time delta) noexcept -> void = 0;

		virtual auto render(sf::RenderWindow& window) noexcept -> void = 0;
	};
}
