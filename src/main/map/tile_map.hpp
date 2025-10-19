#pragma once

#include <utility/matrix.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace map
{
	enum class TileType : std::uint8_t
	{
		// 普通地面(可以通过,不可建造)
		FLOOR = 0,
		// 普通地面(可以通过,可建造)
		BUILDABLE_FLOOR = 1,
		// 障碍物(不可通过,不可建造)
		OBSTACLE = 2,
		// 障碍物(不可通过,可建造)
		BUILDABLE_OBSTACLE = 3,
		// 塔(不可通过,不可建造)(可拆除)
		TOWER = 4,
	};

	class TileMap
	{
	public:
		using data_type = utility::Matrix<TileType>;

		using size_type = data_type::size_type;
		using tile_type = sf::Vector2<size_type>;

		using iterator = data_type::iterator;
		using const_iterator = data_type::const_iterator;

	private:
		size_type tile_width_;
		size_type tile_height_;
		data_type data_;

	public:
		constexpr TileMap() noexcept
			: tile_width_{0},
			  tile_height_{0} {}

		constexpr TileMap(
			const size_type tile_width,
			const size_type tile_height,
			const size_type horizontal_tile_count,
			const size_type vertical_tile_count
		) noexcept
			: tile_width_{tile_width},
			  tile_height_{tile_height},
			  data_{horizontal_tile_count, vertical_tile_count, TileType::FLOOR}
		{
			//
		}

		constexpr TileMap(
			const size_type horizontal_tile_count,
			const size_type vertical_tile_count
		) noexcept
			: TileMap{0, 0, horizontal_tile_count, vertical_tile_count}
		{
			//
		}

		constexpr auto set_tile_width(const size_type new_tile_width) noexcept -> void
		{
			tile_width_ = new_tile_width;
		}

		constexpr auto set_tile_height(const size_type new_tile_height) noexcept -> void
		{
			tile_height_ = new_tile_height;
		}

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
			return tile_width_ * horizontal_tile_count();
		}

		[[nodiscard]] constexpr auto map_height() const noexcept -> size_type
		{
			return tile_height_ * vertical_tile_count();
		}

		[[nodiscard]] constexpr auto map_size() const noexcept -> size_type
		{
			return map_width() * map_height();
		}

		template<typename T = size_type>
		[[nodiscard]] constexpr auto map_bounds() const noexcept -> sf::Rect<T>
		{
			constexpr auto position = sf::Vector2{static_cast<T>(0), static_cast<T>(0)};
			const auto size = sf::Vector2{static_cast<T>(map_width()), static_cast<T>(map_height())};

			return {position, size};
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
		[[nodiscard]] auto find_overlapping_tiles(const sf::FloatRect& bounds) const noexcept -> std::vector<tile_type>;

		// CIRCLE
		[[nodiscard]] auto find_overlapping_tiles(sf::Vector2f center, float radius) const noexcept -> std::vector<tile_type>;

		// OBB
		[[nodiscard]] auto find_overlapping_tiles(const sf::FloatRect& bounds, sf::Angle angle) const noexcept -> std::vector<tile_type>;

		// SECTOR
		[[nodiscard]] auto find_overlapping_tiles(const sf::Vector2f& center, float radius, sf::Angle from, sf::Angle to) const noexcept -> std::vector<tile_type>;

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

		[[nodiscard]] constexpr auto coordinate_world_to_grid(const float x, const float y) const noexcept -> tile_type
		{
			return
			{
					static_cast<size_type>(x / static_cast<float>(tile_width_)),
					static_cast<size_type>(y / static_cast<float>(tile_height_))
			};
		}

		[[nodiscard]] constexpr auto coordinate_world_to_grid(const sf::Vector2f world) const noexcept -> tile_type
		{
			return coordinate_world_to_grid(world.x, world.y);
		}

		[[nodiscard]] constexpr auto inside(const size_type x, const size_type y) const noexcept -> bool
		{
			return x < horizontal_tile_count() && y < vertical_tile_count();
		}

		[[nodiscard]] constexpr auto at(const size_type x, const size_type y) const noexcept -> TileType
		{
			return data_[x, y];
		}

		[[nodiscard]] constexpr auto passable(const TileType type) const noexcept -> bool
		{
			std::ignore = this;

			switch (type)
			{
				case TileType::FLOOR:
				case TileType::BUILDABLE_FLOOR:
				{
					return true;
				}
				case TileType::OBSTACLE:
				case TileType::BUILDABLE_OBSTACLE:
				case TileType::TOWER:
				{
					return false;
				}
			}

			std::unreachable();
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
} // namespace map
