#pragma once

// #include <bit>
#include <cstdint>

namespace game::component::resource
{
	enum class Type : std::uint8_t
	{
		// 生命值
		HEALTH,
		// 魔法值(用于释放技能)
		MANA,
		// 金币
		GOLD,
		//
	};

	// 可以为负数
	using size_type = std::int64_t;

	class Resource
	{
	public:
		// using bits_type = std::uint64_t;
		using bits_type = size_type;

	private:
		bits_type type_ : 8;
		bits_type amount_ : 64 - 8;

	public:
		constexpr Resource(const Type type, const size_type amount) noexcept
			: type_{std::to_underlying(type)},
			  // amount_{std::bit_cast<bits_type>(amount)}
			  amount_{amount} {}

		[[nodiscard]] constexpr auto type() const noexcept -> Type
		{
			return static_cast<Type>(type_);
		}

		[[nodiscard]] constexpr auto amount() const noexcept -> size_type
		{
			// return std::bit_cast<size_type>(amount_);
			return amount_;
		}
	};
}
