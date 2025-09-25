#include <helper/observer.hpp>

#include <components/map.hpp>
#include <components/observer.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>

#include <entt/entt.hpp>

namespace
{
	// 从指定网格中获取所有位置满足contains的敌人实体
	template<typename Contains>
	[[nodiscard]] auto collect_from_tiles(
		entt::registry& registry,
		const components::enemy::Archetype archetype,
		const bool visible_only,
		const std::vector<sf::Vector2<map::TileMap::size_type>>& tiles,
		const Contains& contains
	) noexcept -> std::vector<entt::entity>
	{
		using namespace components;

		const auto& [enemies_ground, enemies_aerial] = registry.ctx().get<const observer::EnemyArchetype>();

		std::vector<entt::entity> all_enemies{};
		const auto emplace_from_bucket = [&](const auto& bucket) noexcept -> void
		{
			std::ranges::for_each(
				bucket,
				[&](const entt::entity entity) noexcept -> void
				{
					if (visible_only and registry.all_of<tags::invisible>(entity))
					{
						return;
					}

					if (const auto [position] = registry.get<const entity::Position>(entity);
						contains(position))
					{
						all_enemies.emplace_back(entity);
					}
				}
			);
		};

		std::ranges::for_each(
			tiles,
			[&](const sf::Vector2<map::TileMap::size_type>& tile) noexcept -> void
			{
				if (std::to_underlying(archetype) & std::to_underlying(enemy::Archetype::GROUND))
				{
					if (const auto it = enemies_ground.find(tile);
						it != enemies_ground.end())
					{
						emplace_from_bucket(it->second);
					}
				}

				if (std::to_underlying(archetype) & std::to_underlying(enemy::Archetype::AERIAL))
				{
					if (const auto it = enemies_aerial.find(tile);
						it != enemies_aerial.end())
					{
						emplace_from_bucket(it->second);
					}
				}
			}
		);

		return all_enemies;
	}

	[[nodiscard]] auto do_select_aabb(
		entt::registry& registry,
		const components::enemy::Archetype archetype,
		const bool visible_only,
		const sf::FloatRect& bounds
	) noexcept -> std::vector<entt::entity>
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		// 获取和AABB重叠的网格
		const auto overlapping_tiles = tile_map.find_overlapping_tiles(bounds);
		// 敌人实体的位置需要处于AABB内
		const auto contains = [
					start_x = bounds.position.x,
					start_y = bounds.position.y,
					end_x = bounds.position.x + bounds.size.x,
					end_y = bounds.position.y + bounds.size.y
				](const sf::Vector2f position) noexcept -> bool
		{
			return
					//
					start_x <= position.x and position.x < end_x and
					//
					start_y <= position.y and position.y < end_y;
		};

		return collect_from_tiles(registry, archetype, visible_only, overlapping_tiles, contains);
	}

	[[nodiscard]] auto do_select_circle(
		entt::registry& registry,
		const components::enemy::Archetype archetype,
		const bool visible_only,
		const sf::Vector2f center,
		const float radius
	) noexcept -> std::vector<entt::entity>
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		// 获取和CIRCLE重叠的网格
		const auto overlapping_tiles = tile_map.find_overlapping_tiles(center, radius);
		// 敌人实体的位置需要处于CIRCLE内
		const auto contains = [
					center,
					r2 = radius * radius
				](const sf::Vector2f position) noexcept -> bool
		{
			const auto dx = center.x - position.x;
			const auto dy = center.y - position.y;
			const auto ds = dx * dx + dy * dy;

			return ds <= r2;
		};

		return collect_from_tiles(registry, archetype, visible_only, overlapping_tiles, contains);
	}

	[[nodiscard]] auto do_select_obb(
		entt::registry& registry,
		const components::enemy::Archetype archetype,
		const bool visible_only,
		const sf::FloatRect& bounds,
		const sf::Angle angle
	) noexcept -> std::vector<entt::entity>
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		const auto theta = -angle.asRadians();

		// 获取和OBB重叠的网格
		const auto overlapping_tiles = tile_map.find_overlapping_tiles(bounds, angle);
		// 敌人实体的位置需要处于OBB内
		const auto contains = [
					center = bounds.getCenter(),
					half_width = bounds.size.x * .5f,
					half_height = bounds.size.y * .5f,
					cos_theta = std::cos(theta),
					sin_theta = std::sin(theta)
				](const sf::Vector2f position) noexcept -> bool
		{
			const auto offset = position - center;
			const auto local = sf::Vector2f
			{
					offset.x * cos_theta - offset.y * sin_theta,
					offset.x * sin_theta + offset.y * cos_theta,
			};

			return std::abs(local.x) <= half_width and std::abs(local.y) <= half_height;
		};

		return collect_from_tiles(registry, archetype, visible_only, overlapping_tiles, contains);
	}

	[[nodiscard]] auto do_select_sector(
		entt::registry& registry,
		const components::enemy::Archetype archetype,
		const bool visible_only,
		const sf::Vector2f center,
		const float radius,
		const sf::Angle from,
		const sf::Angle to
	) noexcept -> std::vector<entt::entity>
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		// 获取和SECTOR重叠的网格
		const auto overlapping_tiles = tile_map.find_overlapping_tiles(center, radius, from, to);
		// 敌人实体的位置需要处于SECTOR内
		const auto contains = [
					center,
					r2 = radius * radius,
					start_radians = from.asRadians(),
					end_radians = to.asRadians()
				](const sf::Vector2f position) noexcept -> bool
		{
			const auto dx = center.x - position.x;
			const auto dy = center.y - position.y;

			if (const auto ds = dx * dx + dy * dy;
				ds > r2)
			{
				return false;
			}

			auto angle = std::atan2(dy, dx);

			if (angle < .0f)
			{
				angle += std::numbers::pi_v<float> * 2;
			}

			if (start_radians <= end_radians)
			{
				if (angle < start_radians or angle > end_radians)
				{
					return false;
				}
			}
			else
			{
				if (angle < start_radians and angle > end_radians)
				{
					return false;
				}
			}

			return true;
		};

		return collect_from_tiles(registry, archetype, visible_only, overlapping_tiles, contains);
	}
}

namespace helper
{
	auto Observer::query(
		entt::registry& registry,
		const components::enemy::Archetype archetype,
		const bool visible_only,
		const sf::FloatRect& bounds
	) noexcept -> std::vector<entt::entity>
	{
		return do_select_aabb(registry, archetype, visible_only, bounds);
	}

	auto Observer::query(
		entt::registry& registry,
		const components::enemy::Archetype archetype,
		const bool visible_only,
		const sf::Vector2f center,
		const float radius
	) noexcept -> std::vector<entt::entity>
	{
		return do_select_circle(registry, archetype, visible_only, center, radius);
	}

	auto Observer::query(
		entt::registry& registry,
		const components::enemy::Archetype archetype,
		const bool visible_only,
		const sf::FloatRect& bounds,
		const sf::Angle angle
	) noexcept -> std::vector<entt::entity>
	{
		return do_select_obb(registry, archetype, visible_only, bounds, angle);
	}

	auto Observer::query(
		entt::registry& registry,
		const components::enemy::Archetype archetype,
		const bool visible_only,
		const sf::Vector2f center,
		const float radius,
		const sf::Angle from,
		const sf::Angle to
	) noexcept -> std::vector<entt::entity>
	{
		return do_select_sector(registry, archetype, visible_only, center, radius, from, to);
	}
}
