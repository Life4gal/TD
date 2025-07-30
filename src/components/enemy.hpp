#pragma once

#include <cstdint>

namespace components::enemy
{
	enum class CategoryValue : std::uint8_t
	{
		GROUND = 0b0000'0001,
		AIR = 0b0000'0010,
	};

	class Category
	{
	public:
		CategoryValue category;
	};

	class Power
	{
	public:
		using value_type = std::uint32_t;

		value_type power;
	};

	class Movement
	{
	public:
		float speed;
	};

	class Health
	{
	public:
		float health;
	};
}
