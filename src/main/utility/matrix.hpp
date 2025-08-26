#pragma once

#include <algorithm>
#include <vector>
#include <span>
#include <ranges>

namespace utility
{
	template<typename T>
	class MatrixView;

	template<typename T>
	class Matrix
	{
	public:
		using element_type = T;
		static_assert(not std::is_const_v<element_type>);

	private:
		using data_type = std::vector<element_type>;

	public:
		using size_type = std::uint32_t;

		using pointer = data_type::pointer;
		using const_pointer = data_type::const_pointer;

		using iterator = data_type::iterator;
		using const_iterator = data_type::const_iterator;

		using reference = data_type::reference;
		using const_reference = data_type::const_reference;

	private:
		size_type width_;
		size_type height_;
		data_type data_;

	public:
		constexpr Matrix() noexcept
			: width_{0},
			  height_{0},
			  data_{} {}

		constexpr Matrix(const size_type width, const size_type height, const element_type& value = {}) noexcept
			: width_{width},
			  height_{height},
			  data_{}
		{
			data_.resize(static_cast<std::size_t>(width_) * height, value);
		}

		[[nodiscard]] constexpr explicit(false) operator MatrixView<T>() noexcept;

		[[nodiscard]] constexpr explicit(false) operator MatrixView<const T>() const noexcept;

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

		[[nodiscard]] constexpr auto data() noexcept -> pointer
		{
			return data_.data();
		}

		[[nodiscard]] constexpr auto data() const noexcept -> const_pointer
		{
			return data_.data();
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

		[[nodiscard]] constexpr auto line(const size_type y) noexcept -> std::span<element_type>
		{
			const auto it = begin() + (y * width());
			const auto length = width();

			return {it, length};
		}

		[[nodiscard]] constexpr auto line(const size_type y) const noexcept -> std::span<const element_type>
		{
			const auto it = begin() + (y * width());
			const auto length = width();

			return {it, length};
		}

		constexpr auto copy_from(const Matrix& other) noexcept -> void;

		constexpr auto move_from(Matrix& other) noexcept -> void;
	};

	template<typename T>
	class MatrixView
	{
	public:
		using element_type = T;

	private:
		using data_type = std::span<element_type>;

	public:
		using size_type = std::uint32_t;

		using pointer = data_type::pointer;
		using const_pointer = data_type::const_pointer;

		using iterator = data_type::iterator;
		using const_iterator = data_type::const_iterator;

		using reference = data_type::reference;
		using const_reference = data_type::const_reference;

	private:
		constexpr static auto is_const = std::is_const_v<element_type>;

		size_type width_;
		size_type height_;
		data_type data_;

	public:
		constexpr MatrixView() noexcept
			: width_{0},
			  height_{0},
			  data_{} {}

		constexpr MatrixView(const size_type width, const size_type height, element_type* value) noexcept
			: width_{width},
			  height_{height},
			  data_{value, static_cast<std::size_t>(width) * height}
		{
			//
		}

		// Matrix<T> ==> MatrixView<T>
		constexpr explicit(false) MatrixView(Matrix<element_type>& matrix) noexcept //
			requires(not is_const)
			: MatrixView{matrix.width(), matrix.height(), matrix.data()} {}

		// Matrix<const T> ==> MatrixView<const T>
		constexpr explicit(false) MatrixView(const Matrix<element_type>& matrix) noexcept //
			requires(is_const)
			: MatrixView{matrix.width(), matrix.height(), matrix.data()} {}

		// MatrixView<T> ==> MatrixView<const T>
		// constexpr explicit(false) MatrixView(const MatrixView<std::remove_const_t<element_type>>& matrix) noexcept //
		// 	requires(is_const)
		// 	: MatrixView{matrix.width(), matrix.height(), matrix.data()} {}

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

		[[nodiscard]] constexpr auto operator[](const size_type x, const size_type y) const noexcept -> const_reference //
			requires (not is_const)
		{
			return data_[y * width() + x];
		}

		[[nodiscard]] constexpr auto data() noexcept -> pointer
		{
			return data_.data();
		}

		[[nodiscard]] constexpr auto data() const noexcept -> const_pointer
		{
			return data_.data();
		}

		[[nodiscard]] constexpr auto begin() noexcept -> iterator
		{
			return data_.begin();
		}

		[[nodiscard]] constexpr auto begin() const noexcept -> const_iterator //
			requires(not is_const)
		{
			return data_.begin();
		}

		[[nodiscard]] constexpr auto end() noexcept -> iterator
		{
			return data_.end();
		}

		[[nodiscard]] constexpr auto end() const noexcept -> const_iterator //
			requires(not is_const)
		{
			return data_.end();
		}

		[[nodiscard]] constexpr auto line(const size_type y) noexcept -> std::span<element_type>
		{
			const auto it = begin() + (y * width());
			const auto length = width();

			return {it, length};
		}

		[[nodiscard]] constexpr auto line(const size_type y) const noexcept -> std::span<const element_type> //
			requires(not is_const)
		{
			const auto it = begin() + (y * width());
			const auto length = width();

			return {it, length};
		}

		constexpr auto copy_from(const MatrixView& other) noexcept -> void //
			requires(not is_const)
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

		constexpr auto move_from(MatrixView& other) noexcept -> void //
			requires(not is_const)
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

	template<typename T>
	constexpr Matrix<T>::operator MatrixView<T>() noexcept
	{
		return MatrixView{*this};
	}

	template<typename T>
	constexpr Matrix<T>::operator MatrixView<const T>() const noexcept
	{
		return MatrixView{*this};
	}

	template<typename T>
	constexpr auto Matrix<T>::copy_from(const Matrix& other) noexcept -> void
	{
		MatrixView self{*this};

		self.copy_from(other);
	}

	template<typename T>
	constexpr auto Matrix<T>::move_from(Matrix& other) noexcept -> void
	{
		MatrixView self{*this};

		self.move_from(other);
	}
}
