#pragma once

#include <span>

#include <components/entity.hpp>
#include <components/resource.hpp>

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems
{
	class Resource
	{
	public:
		static auto initialize(entt::registry& registry) noexcept -> void;

		static auto update(entt::registry& registry) noexcept -> void;

		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;

		// =============================
		// 获取资源

		static auto acquire(entt::registry& registry, std::span<const components::Resource> resources) noexcept -> void;

		static auto acquire(entt::registry& registry, components::Resource resource) noexcept -> entt::entity;

		static auto acquire(entt::registry& registry, std::span<const components::EntityType> types) noexcept -> void;

		static auto acquire(entt::registry& registry, components::EntityType type) noexcept -> entt::entity;

		// =============================
		// 检查资源是否足够

		static auto require(entt::registry& registry, std::span<const components::Resource> resources) noexcept -> bool;

		static auto require(entt::registry& registry, components::Resource resource) noexcept -> bool;

		static auto require(entt::registry& registry, std::span<const components::EntityType> types) noexcept -> bool;

		static auto require(entt::registry& registry, components::EntityType type) noexcept -> bool;

		// =============================
		// 消耗资源(如果足够)

		static auto consume(entt::registry& registry, std::span<const components::Resource> resources) noexcept -> bool;

		static auto consume(entt::registry& registry, components::Resource resource) noexcept -> bool;

		static auto consume(entt::registry& registry, std::span<const components::EntityType> types) noexcept -> bool;

		static auto consume(entt::registry& registry, components::EntityType type) noexcept -> bool;

		// =============================
		// 消耗资源(假定足够)

		static auto consume_unchecked(entt::registry& registry, std::span<const components::Resource> resources) noexcept -> void;

		static auto consume_unchecked(entt::registry& registry, components::Resource resource) noexcept -> void;

		static auto consume_unchecked(entt::registry& registry, std::span<const components::EntityType> types) noexcept -> void;

		static auto consume_unchecked(entt::registry& registry, components::EntityType type) noexcept -> void;
	};
}
