#pragma once

#include <entt/fwd.hpp>

namespace helper
{
	class Map
	{
	public:
		static auto load(entt::registry& registry) noexcept -> void;
	};
}
