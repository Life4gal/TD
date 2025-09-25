#pragma once

#include <algorithm>
#include <numbers>

#include <utility/matrix.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace map
{
	enum class TileType : std::uint8_t
	{
		// 普通地板,可以通行,不可建造
		FLOOR = 0,
		// 可建造地板,可以通行
		BUILDABLE_FLOOR,
		// 固定障碍,不可通行
		OBSTACLE,
		// 建造的塔,不可通行,可拆除
		TOWER,
	};

	class TileMap
	{
	public:
		using data_type = utility::Matrix<TileType>;

		using size_type = data_type::size_type;

		using iterator = data_type::iterator;
		using const_iterator = data_type::const_iterator;

	private:
		size_type tile_width_;
		size_type tile_height_;
		data_type data_;

	public:
		constexpr TileMap() noexcept
			: tile_width_{},
			  tile_height_{} {}

		constexpr TileMap(
			const size_type tile_width,
			const size_type tile_height,
			const size_type horizontal_tile_count,
			const size_type vertical_tile_count
		) noexcept
			: tile_width_{tile_width},
			  tile_height_{tile_height},
			  data_{horizontal_tile_count, vertical_tile_count, TileType::FLOOR} {}

		[[nodiscard]] constexpr auto tile_width() const noexcept -> size_type
		{
			return tile_width_;
		}

		[[nodiscard]] constexpr auto tile_height() const noexcept -> size_type
		{
			return tile_height_;
		}

		[[nodiscard]] constexpr auto horizontal_tile_count() const noexcept -> size_type
		{
			return data_.width();
		}

		[[nodiscard]] constexpr auto vertical_tile_count() const noexcept -> size_type
		{
			return data_.height();
		}

		[[nodiscard]] constexpr auto map_width() const noexcept -> size_type
		{
			return tile_width() * horizontal_tile_count();
		}

		[[nodiscard]] constexpr auto map_height() const noexcept -> size_type
		{
			return tile_height() * vertical_tile_count();
		}

		[[nodiscard]] constexpr auto map_size() const noexcept -> size_type
		{
			return map_width() * map_height();
		}

		template<typename T = size_type>
		[[nodiscard]] constexpr auto tile_bounds(const size_type x, const size_type y) const noexcept -> sf::Rect<T>
		{
			const auto position = sf::Vector2{static_cast<T>(x * tile_width_), static_cast<T>(y * tile_height_)};
			const auto size = sf::Vector2{static_cast<T>(tile_width_), static_cast<T>(tile_height_)};

			return {position, size};
		}

		template<typename T = size_type>
		[[nodiscard]] constexpr auto tile_bounds(const sf::Vector2i grid) const noexcept -> sf::Rect<T>
		{
			return tile_bounds<T>(grid.x, grid.y);
		}

		template<typename T = size_type>
		[[nodiscard]] constexpr auto tile_bounds(const sf::Vector2u grid) const noexcept -> sf::Rect<T>
		{
			return tile_bounds<T>(grid.x, grid.y);
		}

		// AABB
		[[nodiscard]] auto find_overlapping_tiles(const sf::FloatRect& bounds) const noexcept -> std::vector<sf::Vector2<size_type>>
		{
			const auto start = coordinate_world_to_grid(bounds.position);
			const auto end = coordinate_world_to_grid(bounds.position + bounds.size);

			std::vector<sf::Vector2<size_type>> overlapping_tiles{};
			for (auto y = start.y; y <= end.y; ++y)
			{
				for (auto x = start.x; x <= end.x; ++x)
				{
					if (not inside(x, y))
					{
						continue;
					}

					if (const auto tb = tile_bounds<float>(x, y);
						tb.findIntersection(bounds))
					{
						overlapping_tiles.emplace_back(x, y);
					}
				}
			}

			return overlapping_tiles;
		}

		// CIRCLE
		[[nodiscard]] auto find_overlapping_tiles(const sf::Vector2f center, const float radius) const noexcept -> std::vector<sf::Vector2<size_type>>
		{
			const sf::FloatRect circle_bounds{{center.x - radius, center.y - radius}, {radius * 2, radius * 2}};
			const auto potential_tiles = find_overlapping_tiles(circle_bounds);

			const auto r2 = radius * radius;

			std::vector<sf::Vector2<size_type>> overlapping_tiles{};
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

					overlapping_tiles.emplace_back(tile);
				}
			);

			return overlapping_tiles;
		}

		// OBB
		[[nodiscard]] auto find_overlapping_tiles(const sf::FloatRect& bounds, const sf::Angle angle) const noexcept -> std::vector<sf::Vector2<size_type>>
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

			std::vector<sf::Vector2<size_type>> overlapping_tiles{};
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

		// SECTOR
		[[nodiscard]] auto find_overlapping_tiles(
			const sf::Vector2f& center,
			const float radius,
			const sf::Angle from,
			const sf::Angle to
		) const noexcept -> std::vector<sf::Vector2<size_type>>
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

			std::vector<sf::Vector2<size_type>> overlapping_tiles{};
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

		[[nodiscard]] constexpr auto coordinate_grid_to_world(const size_type x, const size_type y) const noexcept -> sf::Vector2<float>
		{
			return
			{
					static_cast<float>(x * tile_width_) + static_cast<float>(tile_width_) / 2.f,
					static_cast<float>(y * tile_height_) + static_cast<float>(tile_height_) / 2.f
			};
		}

		[[nodiscard]] constexpr auto coordinate_grid_to_world(const sf::Vector2i grid) const noexcept -> sf::Vector2<float>
		{
			return coordinate_grid_to_world(grid.x, grid.y);
		}

		[[nodiscard]] constexpr auto coordinate_grid_to_world(const sf::Vector2u grid) const noexcept -> sf::Vector2<float>
		{
			return coordinate_grid_to_world(grid.x, grid.y);
		}

		[[nodiscard]] constexpr auto coordinate_world_to_grid(const float x, const float y) const noexcept -> sf::Vector2<size_type>
		{
			return
			{
					static_cast<size_type>(x / static_cast<float>(tile_width_)),
					static_cast<size_type>(y / static_cast<float>(tile_height_))
			};
		}

		[[nodiscard]] constexpr auto coordinate_world_to_grid(const sf::Vector2f world) const noexcept -> sf::Vector2<size_type>
		{
			return coordinate_world_to_grid(world.x, world.y);
		}

		[[nodiscard]] constexpr auto inside(const size_type x, const size_type y) const noexcept -> bool
		{
			return x < horizontal_tile_count() and y < vertical_tile_count();
		}

		[[nodiscard]] constexpr auto at(const size_type x, const size_type y) const noexcept -> TileType
		{
			return data_[x, y];
		}

		[[nodiscard]] constexpr auto passable(const TileType type) const noexcept -> bool
		{
			std::ignore = this;

			if (type == TileType::FLOOR or type == TileType::BUILDABLE_FLOOR)
			{
				return true;
			}

			return false;
		}

		[[nodiscard]] constexpr auto passable(const size_type x, const size_type y) const noexcept -> bool
		{
			const auto tile = at(x, y);
			return passable(tile);
		}

		constexpr auto set(const size_type x, const size_type y, const TileType type) noexcept -> void
		{
			data_[x, y] = type;
		}

		[[nodiscard]] constexpr auto begin() noexcept -> iterator
		{
			return data_.begin();
		}

		[[nodiscard]] constexpr auto begin() const noexcept -> const_iterator
		{
			return data_.begin();
		}

		[[nodiscard]] constexpr auto end() noexcept -> iterator
		{
			return data_.end();
		}

		[[nodiscard]] constexpr auto end() const noexcept -> const_iterator
		{
			return data_.end();
		}
	};
}
