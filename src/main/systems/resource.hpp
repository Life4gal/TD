#pragma once

#include <entt/fwd.hpp>

namespace systems
{
	class Resource
	{
	public:
		static auto update(entt::registry& registry) noexcept -> void;
	};
}
