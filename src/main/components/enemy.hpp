#pragma once

#include <map/path.hpp>

namespace components::enemy
{
	// 敌人实体不包含该枚举值
	// 其用于Observer限定范围
	enum class Archetype : std::uint8_t
	{
		// 地面移动
		GROUND = 0b0000'0001,
		// 空中移动
		AERIAL = 0b0000'0010,
		// 皆可
		DUAL = GROUND | AERIAL,
	};

	class Health
	{
	public:
		float health;
	};

	class Movement
	{
	public:
		float speed;
	};

	class Power
	{
	public:
		using value_type = std::uint32_t;

		value_type power;
	};

	// 只有地面单位才需要(洋流)方向
	class Direction
	{
	public:
		map::Direction direction;
	};
}
