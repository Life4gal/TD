#include <helper/tower.hpp>

#include <components/core/tags.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Tower::targeting_of(const entt::registry& registry, const entt::entity tower) noexcept -> components::enemy::Archetype
	{
		using namespace components;

		// 索敌类型
		const auto targeting_ground = registry.all_of<tags::targeting_ground>(tower);
		const auto targeting_air = registry.all_of<tags::targeting_air>(tower);

		if (targeting_ground and targeting_air)
		{
			return enemy::Archetype::DUAL;
		}

		if (targeting_ground)
		{
			return enemy::Archetype::GROUND;
		}

		if (targeting_air)
		{
			return enemy::Archetype::AERIAL;
		}

		std::unreachable();
	}
}
