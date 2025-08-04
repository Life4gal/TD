#pragma once

#include <cstdint>

namespace game::component::enemy
{
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
