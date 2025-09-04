#pragma once

#include <entt/fwd.hpp>

namespace systems
{
	class Initialize
	{
	public:
		static auto initialize(entt::registry& registry) noexcept -> void;
	};
}
