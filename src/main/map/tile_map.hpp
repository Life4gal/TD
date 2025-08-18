#pragma once

#include <utility/matrix.hpp>

#include <SFML/System/Vector2.hpp>

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

		[[nodiscard]] constexpr auto coordinate_grid_to_world(const sf::Vector2i grid) const noexcept -> sf::Vector2f
		{
			return
			{
					static_cast<float>(grid.x * tile_width()) + static_cast<float>(tile_width()) / 2.f,
					static_cast<float>(grid.y * tile_height()) + static_cast<float>(tile_height()) / 2.f
			};
		}

		[[nodiscard]] constexpr auto coordinate_grid_to_world(const sf::Vector2u grid) const noexcept -> sf::Vector2f
		{
			return
			{
					static_cast<float>(grid.x * tile_width()) + static_cast<float>(tile_width()) / 2.f,
					static_cast<float>(grid.y * tile_height()) + static_cast<float>(tile_height()) / 2.f
			};
		}

		[[nodiscard]] constexpr auto coordinate_world_to_grid(const sf::Vector2f world) const noexcept -> sf::Vector2u
		{
			return
			{
					static_cast<unsigned>(world.x / static_cast<float>(tile_width())),
					static_cast<unsigned>(world.y / static_cast<float>(tile_height()))
			};
		}

		[[nodiscard]] constexpr auto coordinate_world_to_grid(const sf::Vector2i world) const noexcept -> sf::Vector2u
		{
			return
			{
					static_cast<unsigned>(world.x) / tile_width(),
					static_cast<unsigned>(world.y) / tile_height()
			};
		}

		[[nodiscard]] constexpr auto coordinate_world_to_grid(const sf::Vector2u world) const noexcept -> sf::Vector2u
		{
			return
			{
					static_cast<unsigned>(world.x) / tile_width(),
					static_cast<unsigned>(world.y) / tile_height()
			};
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
