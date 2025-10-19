#pragma once

#include <span>

#include <entt/entity/sparse_set.hpp>

namespace helper
{
	class Entity
	{
	public:
		// 销毁实体,不执行回调
		static auto kill(entt::registry& registry, entt::entity entity) noexcept -> void;

		// 销毁实体,不执行回调
		static auto kill(entt::registry& registry, std::span<const entt::entity> entities) noexcept -> void;

		// 销毁实体,不执行回调
		static auto kill(
			entt::registry& registry,
			entt::internal::sparse_set_iterator<std::vector<entt::entity>> begin,
			entt::internal::sparse_set_iterator<std::vector<entt::entity>> end
		) noexcept -> void;

		// 销毁实体,如果有回调则执行回调
		static auto destroy(entt::registry& registry, entt::entity entity) noexcept -> void;

		// 销毁实体,如果有回调则执行回调
		static auto destroy(entt::registry& registry, std::span<const entt::entity> entities) noexcept -> void;

		// 销毁实体,如果有回调则执行回调
		static auto destroy(
			entt::registry& registry,
			entt::internal::sparse_set_iterator<std::vector<entt::entity>> begin,
			entt::internal::sparse_set_iterator<std::vector<entt::entity>> end
		) noexcept -> void;
	};
}
