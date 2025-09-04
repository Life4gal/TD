#pragma once

#include <components/wave.hpp>

#include <entt/fwd.hpp>

namespace helper
{
	class Wave
	{
	public:
		// 载入波次数据
		static auto load(entt::registry& registry) noexcept -> void;

		// 是否还有下一波次
		static auto has_next_wave(entt::registry& registry) noexcept -> bool;

		// 生成指定波次(仅此而已)
		static auto spawn(entt::registry& registry, components::wave::WaveIndex wave_index) noexcept -> void;

		// 从指定波次开始
		static auto start_at(entt::registry& registry, components::wave::WaveIndex wave_index) noexcept -> void;

		// 开始波次(基于设定的波次索引)
		static auto start(entt::registry& registry) noexcept -> void;
	};
}
