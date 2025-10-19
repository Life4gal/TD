#include <helper/entity.hpp>

#include <algorithm>

#include <components/entity.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Entity::kill(entt::registry& registry, const entt::entity entity) noexcept -> void
	{
		using namespace components;

		registry.destroy(entity);
	}

	auto Entity::kill(entt::registry& registry, const std::span<const entt::entity> entities) noexcept -> void
	{
		using namespace components;

		registry.destroy(entities.begin(), entities.end());
	}

	auto Entity::kill(
		entt::registry& registry,
		const entt::internal::sparse_set_iterator<std::vector<entt::entity>> begin,
		const entt::internal::sparse_set_iterator<std::vector<entt::entity>> end
	) noexcept -> void
	{
		using namespace components;

		registry.destroy(begin, end);
	}

	auto Entity::destroy(entt::registry& registry, const entt::entity entity) noexcept -> void
	{
		using namespace components;

		if (const auto* on_death = registry.try_get<const entity::OnDeath>(entity))
		{
			on_death->on_death(registry, entity);
		}

		kill(registry, entity);
	}

	auto Entity::destroy(entt::registry& registry, const std::span<const entt::entity> entities) noexcept -> void
	{
		using namespace components;

		std::ranges::for_each(
			entities,
			[&](const entt::entity entity) noexcept -> void
			{
				if (const auto* on_death = registry.try_get<const entity::OnDeath>(entity))
				{
					on_death->on_death(registry, entity);
				}
			}
		);

		kill(registry, entities);
	}

	auto Entity::destroy(
		entt::registry& registry,
		const entt::internal::sparse_set_iterator<std::vector<entt::entity>> begin,
		const entt::internal::sparse_set_iterator<std::vector<entt::entity>> end
	) noexcept -> void
	{
		using namespace components;

		std::ranges::for_each(
			std::ranges::subrange{begin, end},
			[&](const entt::entity entity) noexcept -> void
			{
				if (const auto* on_death = registry.try_get<const entity::OnDeath>(entity))
				{
					on_death->on_death(registry, entity);
				}
			}
		);

		kill(registry, begin, end);
	}
}
