#pragma once

#include <entt/fwd.hpp>

namespace systems
{
	class Graveyard
	{
	public:
		static auto update(entt::registry& registry) noexcept -> void;
	};
}
