#pragma once

#include <entt/fwd.hpp>

#include <SFML/System/Time.hpp>

namespace systems
{
	class SpriteFrame
	{
	public:
		static auto update(entt::registry& registry, sf::Time delta) noexcept -> void;
	};
}
