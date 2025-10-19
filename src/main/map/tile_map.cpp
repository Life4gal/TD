#include <map/tile_map.hpp>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace map
{
	auto TileMap::find_overlapping_tiles(const sf::FloatRect& bounds) const noexcept -> std::vector<tile_type>
	{
		const auto bound_start = coordinate_world_to_grid(bounds.position);
		const auto bound_end = coordinate_world_to_grid(bounds.position + bounds.size);

		if (bound_start.x >= horizontal_tile_count() or bound_start.y >= vertical_tile_count())
		{
			return {};
		}

		const auto start_x = std::ranges::max(static_cast<size_type>(0), bound_start.x);
		const auto end_x = std::ranges::min(horizontal_tile_count() - 1, bound_end.x);

		const auto start_y = std::ranges::max(static_cast<size_type>(0), bound_start.y);
		const auto end_y = std::ranges::min(vertical_tile_count() - 1, bound_end.y);

		const auto tiles_count = static_cast<std::size_t>(end_x - start_x + 1) * (end_y - start_y + 1);

		std::vector<tile_type> overlapping_tiles{};
		overlapping_tiles.reserve(tiles_count);

		for (auto y = start_y; y <= end_y; ++y)
		{
			for (auto x = start_x; x <= end_x; ++x)
			{
				overlapping_tiles.emplace_back(x, y);
			}
		}

		return overlapping_tiles;
	}

	auto TileMap::find_overlapping_tiles(const sf::Vector2f center, const float radius) const noexcept -> std::vector<tile_type>
	{
		const auto circle_bounds = sf::FloatRect{{center.x - radius, center.y - radius}, {radius * 2, radius * 2}};
		const auto potential_tiles = find_overlapping_tiles(circle_bounds);

		const auto radius_2 = radius * radius;

		std::vector<tile_type> overlapping_tiles{};
		std::ranges::for_each(
			potential_tiles,
			[&](const auto& tile) noexcept -> void
			{
				const auto tb = this->tile_bounds<float>(tile);

				const auto closest_x = std::ranges::max(
					tb.position.x,
					std::ranges::min(center.x, tb.position.x + tb.size.x)
				);
				const auto closest_y = std::ranges::max(
					tb.position.y,
					std::ranges::min(center.y, tb.position.y + tb.size.y)
				);

				const auto dx = center.x - closest_x;
				const auto dy = center.y - closest_y;

				if (const auto distance_2 = dx * dx + dy * dy;
					distance_2 > radius_2)
				{
					return;
				}

				overlapping_tiles.emplace_back(tile);
			}
		);

		return overlapping_tiles;
	}

	auto TileMap::find_overlapping_tiles(const sf::FloatRect& bounds, const sf::Angle angle) const noexcept -> std::vector<tile_type>
	{
		const auto radians = angle.asRadians();
		const auto cos_rotation = std::cos(radians);
		const auto sin_rotation = std::sin(radians);

		const auto center = bounds.getCenter();
		const auto half_width = bounds.size.x / 2.f;
		const auto half_height = bounds.size.y / 2.f;

		const float corners_x[]
		{
				center.x + cos_rotation * -half_width - sin_rotation * -half_height,
				center.x + cos_rotation * half_width - sin_rotation * -half_height,
				center.x + cos_rotation * half_width - sin_rotation * half_height,
				center.x + cos_rotation * -half_width - sin_rotation * half_height,
		};
		const float corners_y[]
		{
				center.y + sin_rotation * -half_width + cos_rotation * -half_height,
				center.y + sin_rotation * half_width + cos_rotation * -half_height,
				center.y + sin_rotation * half_width + cos_rotation * half_height,
				center.y + sin_rotation * -half_width + cos_rotation * half_height,
		};

		const auto [min_x, max_x] = std::ranges::minmax(corners_x);
		const auto [min_y, max_y] = std::ranges::minmax(corners_y);

		const sf::FloatRect aabb_bounds{{min_x, min_y}, {max_x - min_x, max_y - min_y}};
		const auto potential_tiles = find_overlapping_tiles(aabb_bounds);

		std::vector<tile_type> overlapping_tiles{};
		std::ranges::for_each(
			potential_tiles,
			[&](const auto& tile) noexcept -> void
			{
				const auto tb = this->tile_bounds<float>(tile);
				const auto tb_center = tb.getCenter();

				const auto center_offset = tb_center - center;
				const auto rotated_position = sf::Vector2f
				{
						center_offset.x * cos_rotation + center_offset.y * sin_rotation,
						-center_offset.x * sin_rotation + center_offset.y * cos_rotation,
				};

				if (std::abs(rotated_position.x) > half_width or std::abs(rotated_position.y) > half_height)
				{
					return;
				}

				overlapping_tiles.emplace_back(tile);
			}
		);

		return overlapping_tiles;
	}

	auto TileMap::find_overlapping_tiles(const sf::Vector2f& center, const float radius, const sf::Angle from, const sf::Angle to) const noexcept -> std::vector<tile_type>
	{
		const auto start_radians = from.asRadians();
		const auto end_radians = to.asRadians();

		std::vector<sf::Vector2f> sector_points{};
		sector_points.emplace_back(center);

		const auto segment_count = std::ranges::max(
			4,
			static_cast<int>(
				// 15度一个点
				(end_radians - start_radians) * (180.f / std::numbers::pi_v<float> / 15.f)
			)
		);
		for (int i = 0; i <= segment_count; ++i)
		{
			const auto angle =
					start_radians +
					(end_radians - start_radians) * static_cast<float>(i) / static_cast<float>(segment_count);

			sector_points.emplace_back(
				center.x + radius * std::cos(angle),
				center.y + radius * std::sin(angle)
			);
		}

		const auto min_x = std::ranges::min(sector_points, {}, &sf::Vector2f::x).x;
		const auto max_x = std::ranges::max(sector_points, {}, &sf::Vector2f::x).x;
		const auto min_y = std::ranges::min(sector_points, {}, &sf::Vector2f::y).y;
		const auto max_y = std::ranges::max(sector_points, {}, &sf::Vector2f::y).y;

		const sf::FloatRect sector_bounds{{min_x, min_y}, {max_x - min_x, max_y - min_y}};
		const auto potential_tiles = find_overlapping_tiles(sector_bounds);

		const auto r2 = radius * radius;

		std::vector<tile_type> overlapping_tiles{};
		std::ranges::for_each(
			potential_tiles,
			[&](const auto& tile) noexcept -> void
			{
				const auto tb = this->tile_bounds<float>(tile);

				const auto closest_x = std::ranges::max(
					tb.position.x,
					std::ranges::min(center.x, tb.position.x + tb.size.x)
				);
				const auto closest_y = std::ranges::max(
					tb.position.y,
					std::ranges::min(center.y, tb.position.y + tb.size.y)
				);

				const auto dx = center.x - closest_x;
				const auto dy = center.y - closest_y;

				if (const auto ds = dx * dx + dy * dy;
					ds > r2)
				{
					return;
				}

				const auto direction = sf::Vector2f{closest_x - center.x, closest_y - center.y};
				auto angle = std::atan2(direction.y, direction.x);

				if (angle < .0f)
				{
					angle += std::numbers::pi_v<float> * 2;
				}

				if (start_radians <= end_radians)
				{
					if (angle < start_radians or angle > end_radians)
					{
						return;
					}
				}
				else
				{
					if (angle < start_radians and angle > end_radians)
					{
						return;
					}
				}

				overlapping_tiles.emplace_back(tile);
			}
		);

		return overlapping_tiles;
	}
}
