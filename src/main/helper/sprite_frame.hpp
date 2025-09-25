#pragma once

#include <entt/fwd.hpp>

#include <SFML/Graphics/Rect.hpp>

namespace helper
{
	class SpriteFrame
	{
	public:
		[[nodiscard]] static auto rect_of(entt::registry& registry, entt::entity entity) noexcept -> sf::IntRect;
	};
}
