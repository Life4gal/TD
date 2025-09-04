#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class RenderWindow;
}

namespace systems
{
	class Map
	{
	public:
		// 更新
		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
	};
}
