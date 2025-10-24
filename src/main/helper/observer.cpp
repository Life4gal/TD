#include <helper/observer.hpp>

#include <components/core/tags.hpp>
#include <components/core/transform.hpp>
#include <components/combat/enemy.hpp>
#include <components/combat/weapon.hpp>
#include <components/map/map.hpp>
#include <components/map/observer.hpp>

#include <helper/tower.hpp>

#include <entt/entt.hpp>

namespace
{
	// // 敌人实体的位置需要处于AABB内
	// const auto contains = [
	// 			start_x = bounds.position.x,
	// 			start_y = bounds.position.y,
	// 			end_x = bounds.position.x + bounds.size.x,
	// 			end_y = bounds.position.y + bounds.size.y
	// 		](const sf::Vector2f position) noexcept -> bool
	// {
	// 	return
	// 			//
	// 			start_x <= position.x and position.x < end_x and
	// 			//
	// 			start_y <= position.y and position.y < end_y;
	// };
	//
	// // 敌人实体的位置需要处于CIRCLE内
	// const auto contains = [
	// 			center,
	// 			r2 = radius * radius
	// 		](const sf::Vector2f position) noexcept -> bool
	// {
	// 	const auto dx = center.x - position.x;
	// 	const auto dy = center.y - position.y;
	// 	const auto ds = dx * dx + dy * dy;
	//
	// 	return ds <= r2;
	// };
	//
	// // 敌人实体的位置需要处于OBB内
	// const auto contains = [
	// 			center = bounds.getCenter(),
	// 			half_width = bounds.size.x * .5f,
	// 			half_height = bounds.size.y * .5f,
	// 			cos_theta = std::cos(theta),
	// 			sin_theta = std::sin(theta)
	// 		](const sf::Vector2f position) noexcept -> bool
	// {
	// 	const auto offset = position - center;
	// 	const auto local = sf::Vector2f
	// 	{
	// 			offset.x * cos_theta - offset.y * sin_theta,
	// 			offset.x * sin_theta + offset.y * cos_theta,
	// 	};
	//
	// 	return std::abs(local.x) <= half_width and std::abs(local.y) <= half_height;
	// };
	//
	// // 敌人实体的位置需要处于SECTOR内
	// const auto contains = [
	// 			center,
	// 			r2 = radius * radius,
	// 			start_radians = from.asRadians(),
	// 			end_radians = to.asRadians()
	// 		](const sf::Vector2f position) noexcept -> bool
	// {
	// 	const auto dx = center.x - position.x;
	// 	const auto dy = center.y - position.y;
	//
	// 	if (const auto ds = dx * dx + dy * dy;
	// 		ds > r2)
	// 	{
	// 		return false;
	// 	}
	//
	// 	auto angle = std::atan2(dy, dx);
	//
	// 	if (angle < .0f)
	// 	{
	// 		angle += std::numbers::pi_v<float> * 2;
	// 	}
	//
	// 	if (start_radians <= end_radians)
	// 	{
	// 		if (angle < start_radians or angle > end_radians)
	// 		{
	// 			return false;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (angle < start_radians and angle > end_radians)
	// 		{
	// 			return false;
	// 		}
	// 	}
	//
	// 	return true;
	// };

	[[nodiscard]] auto do_search_region(
		entt::registry& registry,
		const bool visible_only,
		const components::enemy::Archetype archetype,
		const sf::Vector2f center,
		const float radius
	) noexcept -> std::vector<helper::Observer::Result>
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();
		const auto overlapping_tiles = tile_map.find_overlapping_tiles(center, radius);

		const auto radius_2 = radius * radius;

		std::vector<helper::Observer::Result> result{};

		// 用于暂时保存所有重叠网格(便于计算扩容)
		std::vector<std::reference_wrapper<const std::vector<entt::entity>>> temp_tiles{};
		temp_tiles.reserve(overlapping_tiles.size());

		const auto do_emplace = [&](const auto& buckets) noexcept -> void
		{
			const auto emplace_from_bucket = [&](const std::vector<entt::entity>& bucket) noexcept -> void
			{
				std::ranges::for_each(
					bucket,
					[&](const entt::entity enemy) noexcept -> void
					{
						if (visible_only and registry.all_of<tags::invisible>(enemy))
						{
							return;
						}

						const auto [position] = registry.get<const transform::Position>(enemy);
						const auto dp = position - center;

						if (const auto distance_2 = dp.lengthSquared();
							distance_2 <= radius_2)
						{
							result.emplace_back(enemy, distance_2);
						}
					}
				);
			};

			// 清理之前保存的tiles
			temp_tiles.clear();

			// 暂时保存所有重叠网格
			std::ranges::for_each(
				overlapping_tiles,
				[&](const map::TileMap::tile_type tile) noexcept -> void
				{
					if (const auto it = buckets.find(tile);
						it != buckets.end())
					{
						temp_tiles.emplace_back(std::cref(it->second));
					}
				}
			);

			// 扩容
			const auto total = std::ranges::fold_left(
				temp_tiles,
				std::size_t{0},
				[&](const std::size_t current, const std::vector<entt::entity>& bucket) noexcept -> std::size_t
				{
					return current + bucket.size();
				}
			);
			result.reserve(result.size() + total);

			// 加入结果集
			std::ranges::for_each(temp_tiles, emplace_from_bucket);
		};

		// 地面单位
		if (std::to_underlying(archetype) & std::to_underlying(enemy::Archetype::GROUND))
		{
			const auto& [ground_enemy, _] = registry.ctx().get<const observer::GroundEnemy>();

			do_emplace(ground_enemy);
		}

		// 空中单位
		if (std::to_underlying(archetype) & std::to_underlying(enemy::Archetype::AERIAL))
		{
			const auto& [aerial_enemy, _] = registry.ctx().get<const observer::AerialEnemy>();

			do_emplace(aerial_enemy);
		}

		return result;
	}
}

namespace helper
{
	auto Observer::search_region(
		entt::registry& registry,
		const entt::entity tower,
		const bool visible_only,
		const sf::Vector2f center,
		const float radius
	) noexcept -> std::vector<Result>
	{
		using namespace components;

		// 索敌类型
		const auto targeting = Tower::targeting_of(registry, tower);

		return do_search_region(registry, visible_only, targeting, center, radius);
	}

	auto Observer::find_tower_target(entt::registry& registry, const entt::entity tower, const sf::Vector2f position, const float range) noexcept -> entt::entity
	{
		using namespace components;

		// 索敌策略
		// 可以有任意一个优先,但是不能是都优先
		assert((not registry.all_of<tags::strategy_ground_first, tags::strategy_air_first>(tower)));
		const auto strategy_ground_first = registry.all_of<tags::strategy_ground_first>(tower);
		const auto strategy_air_first = registry.all_of<tags::strategy_air_first>(tower);
		// 索敌类型
		const auto targeting = Tower::targeting_of(registry, tower);

		if (strategy_ground_first)
		{
			assert(std::to_underlying(targeting) & std::to_underlying(enemy::Archetype::GROUND));
		}
		if (strategy_air_first)
		{
			assert(std::to_underlying(targeting) & std::to_underlying(enemy::Archetype::AERIAL));
		}

		const auto enemies_in_range = [&]() noexcept -> std::vector<Result>
		{
			const auto do_search = [&](const enemy::Archetype archetype) noexcept -> std::vector<Result>
			{
				return do_search_region(registry, true, archetype, position, range);
			};

			// 同时对地/对空
			if (targeting == enemy::Archetype::DUAL)
			{
				if (strategy_ground_first)
				{
					if (auto e = do_search(enemy::Archetype::GROUND);
						not e.empty())
					{
						return e;
					}

					return do_search(enemy::Archetype::AERIAL);
				}

				if (strategy_air_first)
				{
					if (auto e = do_search(enemy::Archetype::AERIAL);
						not e.empty())
					{
						return e;
					}

					return do_search(enemy::Archetype::GROUND);
				}

				return do_search(enemy::Archetype::DUAL);
			}

			// 仅对地/对空
			return do_search(targeting);
		}();

		if (enemies_in_range.empty())
		{
			return entt::null;
		}

		// 索敌策略
		// 必须至少有一个优先,但是不能是都优先
		assert((registry.any_of<tags::strategy_distance_first, tags::strategy_power_first>(tower)));
		assert((not registry.all_of<tags::strategy_distance_first, tags::strategy_power_first>(tower)));
		const auto strategy_distance_first = registry.all_of<tags::strategy_distance_first>(tower);
		const auto strategy_power_first = registry.all_of<tags::strategy_power_first>(tower);

		// 距离优先
		if (strategy_distance_first)
		{
			const auto [entity, distance_2] = std::ranges::min(
				enemies_in_range,
				std::ranges::less{},
				&Result::distance_2
			);

			return entity;
		}

		// 强度优先
		if (strategy_power_first)
		{
			const auto max_power = std::ranges::max(
				enemies_in_range | std::views::transform(&Result::entity),
				std::ranges::greater{},
				[&](const entt::entity enemy) noexcept -> enemy::Power::value_type
				{
					return registry.get<const enemy::Power>(enemy).power;
				}
			);

			return max_power;
		}

		std::unreachable();
	}

	auto Observer::find_tower_target(entt::registry& registry, const entt::entity tower) noexcept -> entt::entity
	{
		using namespace components;

		// 塔位置(中心点)
		const auto [position] = registry.get<const transform::Position>(tower);
		// 搜索范围(攻击范围)
		const auto [range] = registry.get<const weapon::Range>(tower);

		return find_tower_target(registry, tower, position, range);
	}
}
