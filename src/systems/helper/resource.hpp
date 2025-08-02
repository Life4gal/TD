#pragma once

#include <span>

#include <components/entity.hpp>
#include <components/resource.hpp>

#include <entt/fwd.hpp>

namespace systems::helper
{
	class Resource
	{
	public:
		// =============================
		// 获取资源

		static auto acquire(entt::registry& registry, std::span<const components::resource::Resource> resources) noexcept -> void;

		static auto acquire(entt::registry& registry, components::resource::Resource resource) noexcept -> entt::entity;

		static auto acquire(entt::registry& registry, std::span<const components::EntityType> types) noexcept -> void;

		static auto acquire(entt::registry& registry, components::EntityType type) noexcept -> entt::entity;

		// =============================
		// 检查资源是否足够

		static auto require(entt::registry& registry, std::span<const components::resource::Resource> resources) noexcept -> bool;

		static auto require(entt::registry& registry, components::resource::Resource resource) noexcept -> bool;

		static auto require(entt::registry& registry, std::span<const components::EntityType> types) noexcept -> bool;

		static auto require(entt::registry& registry, components::EntityType type) noexcept -> bool;

		// =============================
		// 消耗资源(如果足够)

		static auto consume(entt::registry& registry, std::span<const components::resource::Resource> resources) noexcept -> bool;

		static auto consume(entt::registry& registry, components::resource::Resource resource) noexcept -> bool;

		static auto consume(entt::registry& registry, std::span<const components::EntityType> types) noexcept -> bool;

		static auto consume(entt::registry& registry, components::EntityType type) noexcept -> bool;

		// =============================
		// 消耗资源(假定足够)

		static auto consume_unchecked(entt::registry& registry, std::span<const components::resource::Resource> resources) noexcept -> void;

		static auto consume_unchecked(entt::registry& registry, components::resource::Resource resource) noexcept -> void;

		static auto consume_unchecked(entt::registry& registry, std::span<const components::EntityType> types) noexcept -> void;

		static auto consume_unchecked(entt::registry& registry, components::EntityType type) noexcept -> void;
	};
}
