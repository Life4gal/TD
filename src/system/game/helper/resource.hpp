#pragma once

#include <span>

#include <component/game/entity.hpp>
#include <component/game/resource.hpp>

#include <entt/fwd.hpp>

namespace game::system::helper
{
	class Resource
	{
	public:
		using resource_type = component::resource::Resource;

		// ===========================
		// 获取资源

		// ==========
		// 直接获取资源

		static auto acquire(entt::registry& registry, std::span<const resource_type> resources) noexcept -> void;
		static auto acquire(entt::registry& registry, resource_type resource) noexcept -> void;

		// ==========
		// 基于实体类型获取资源(击杀敌人或者拆除塔)

		static auto acquire(entt::registry& registry, std::span<const component::entity::Type> types) noexcept -> void;
		static auto acquire(entt::registry& registry, component::entity::Type type) noexcept -> void;

		// ==========
		// 基于实体获取资源(击杀敌人或者拆除塔)

		static auto acquire(entt::registry& registry, std::span<const entt::entity> entities) noexcept -> void;
		static auto acquire(entt::registry& registry, entt::entity entity) noexcept -> void;

		// ===========================
		// 检查资源是否足够

		// ==========
		// 直接检查资源

		static auto require(entt::registry& registry, std::span<const resource_type> resources) noexcept -> bool;
		static auto require(entt::registry& registry, resource_type resource) noexcept -> bool;

		// ==========
		// 基于实体类型检查资源(建造塔或装备武器)

		static auto require(entt::registry& registry, std::span<const component::entity::Type> types) noexcept -> bool;
		static auto require(entt::registry& registry, component::entity::Type type) noexcept -> bool;

		// ==========
		// 基于实体检查资源(建造塔或装备武器)

		static auto require(entt::registry& registry, std::span<const entt::entity> entities) noexcept -> bool;
		static auto require(entt::registry& registry, entt::entity entity) noexcept -> bool;

		// ===========================
		// 消耗资源(如果足够)

		// ==========
		// 直接消耗资源

		static auto consume(entt::registry& registry, std::span<const resource_type> resources) noexcept -> bool;
		static auto consume(entt::registry& registry, resource_type resource) noexcept -> bool;

		// ==========
		// 基于实体类型消耗资源(建造塔或装备武器)

		static auto consume(entt::registry& registry, std::span<const component::entity::Type> types) noexcept -> bool;
		static auto consume(entt::registry& registry, component::entity::Type type) noexcept -> bool;

		// ==========
		// 基于实体消耗资源(建造塔或装备武器)

		static auto consume(entt::registry& registry, std::span<const entt::entity> entities) noexcept -> bool;
		static auto consume(entt::registry& registry, entt::entity entity) noexcept -> bool;

		// ===========================
		// 消耗资源(假定足够)

		// ==========
		// 直接消耗资源

		static auto consume_unchecked(entt::registry& registry, std::span<const resource_type> resources) noexcept -> void;
		static auto consume_unchecked(entt::registry& registry, resource_type resource) noexcept -> void;

		// ==========
		// 基于实体类型消耗资源(建造塔或装备武器)

		static auto consume_unchecked(entt::registry& registry, std::span<const component::entity::Type> types) noexcept -> void;
		static auto consume_unchecked(entt::registry& registry, component::entity::Type type) noexcept -> void;

		// ==========
		// 基于实体消耗资源(建造塔或装备武器)

		static auto consume_unchecked(entt::registry& registry, std::span<const entt::entity> entities) noexcept -> void;
		static auto consume_unchecked(entt::registry& registry, entt::entity entity) noexcept -> void;
	};
}
