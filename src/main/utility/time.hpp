#pragma once

#include <chrono>

namespace utility
{
	[[nodiscard]] inline auto zoned_now() noexcept -> std::chrono::zoned_time<std::chrono::system_clock::duration>
	{
		const auto time = std::chrono::system_clock::now();
		const auto local_time = std::chrono::zoned_time{std::chrono::current_zone(), time};
		return local_time;
	}
}
