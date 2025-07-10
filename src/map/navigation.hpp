#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

namespace map
{
	class Navigation
	{
	public:
		enum class AgentId : std::uint32_t {};

		Navigation(const Navigation&) noexcept = delete;
		Navigation(Navigation&&) noexcept = delete;
		auto operator=(const Navigation&) noexcept -> Navigation& = delete;
		auto operator=(Navigation&&) noexcept -> Navigation& = delete;

		virtual ~Navigation() noexcept;

	protected:
		Navigation() noexcept;

	public:
		[[nodiscard]] virtual auto add_agent(entt::registry& registry, sf::Vector2f start_world_position, sf::Vector2f end_world_position) noexcept -> AgentId = 0;

		virtual auto remove_agent(entt::registry& registry, AgentId id) noexcept -> bool = 0;

		virtual auto set_agent_target(entt::registry& registry, AgentId id, sf::Vector2f new_end_world_position) noexcept -> bool = 0;

		[[nodiscard]] virtual auto has_next_position(entt::registry& registry, AgentId id) const noexcept -> bool = 0;

		[[nodiscard]] virtual auto get_next_position(entt::registry& registry, AgentId id) const noexcept -> sf::Vector2f = 0;

		virtual auto move_to_next_position(entt::registry& registry, AgentId id) noexcept -> void = 0;

		virtual auto update(entt::registry& registry, sf::Time delta) noexcept -> void = 0;
	};
}
