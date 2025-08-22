#pragma once

#include <vector>
#include <span>

namespace utility
{
	template<typename T>
	class Matrix
	{
	public:
		using value_type = T;
		using data_type = std::vector<value_type>;

		using size_type = std::uint32_t;

		using iterator = typename data_type::iterator;
		using const_iterator = typename data_type::const_iterator;

		using reference = typename data_type::reference;
		using const_reference = typename data_type::const_reference;

	private:
		size_type width_;
		size_type height_;
		data_type data_;

	public:
		constexpr Matrix() noexcept
			: width_{0},
			  height_{0},
			  data_{} {}

		constexpr Matrix(const size_type width, const size_type height, const value_type& value = {}) noexcept
			: width_{width},
			  height_{height},
			  data_{}
		{
			data_.resize(static_cast<std::size_t>(width_) * height_, value);
		}

		[[nodiscard]] constexpr auto size() const noexcept -> size_type
		{
			return static_cast<size_type>(data_.size());
		}

		[[nodiscard]] constexpr auto empty() const noexcept -> bool
		{
			return data_.empty();
		}

		[[nodiscard]] constexpr auto width() const noexcept -> size_type
		{
			return width_;
		}

		[[nodiscard]] constexpr auto height() const noexcept -> size_type
		{
			return height_;
		}

		[[nodiscard]] constexpr auto operator[](const size_type x, const size_type y) noexcept -> reference
		{
			return data_[y * width() + x];
		}

		[[nodiscard]] constexpr auto operator[](const size_type x, const size_type y) const noexcept -> const_reference
		{
			return data_[y * width() + x];
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

		[[nodiscard]] constexpr auto line(const size_type y) noexcept -> std::span<value_type>
		{
			const auto it = begin() + (y * width());
			const auto length = width();

			return {it, length};
		}

		[[nodiscard]] constexpr auto line(const size_type y) const noexcept -> std::span<const value_type>
		{
			const auto it = begin() + (y * width());
			const auto length = width();

			return {it, length};
		}

		constexpr auto copy_from(const Matrix& other) noexcept -> void
		{
			const auto min_height = std::ranges::min(height(), other.height());
			const auto min_width = std::ranges::min(width(), other.width());

			for (size_type y = 0; y < min_height; ++y)
			{
				const auto from = other.line(y);
				auto to = line(y);

				std::ranges::copy(
					std::ranges::subrange
					{
							from.begin(),
							from.begin() + min_width
					},
					to.data()
				);
			}
		}

		constexpr auto move_from(const Matrix& other) noexcept -> void
		{
			const auto min_height = std::ranges::min(height(), other.height());
			const auto min_width = std::ranges::min(width(), other.width());

			for (size_type y = 0; y < min_height; ++y)
			{
				const auto from = other.line(y);
				auto to = line(y);

				std::ranges::copy(
					std::ranges::subrange
					{
							std::make_move_iterator(from.begin()),
							std::make_move_iterator(from.begin() + min_width)
					},
					to.data()
				);
			}
		}
	};
}
