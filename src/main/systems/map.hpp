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
		// 载入地图
		static auto load(entt::registry& registry) noexcept -> void;

		// 初始化
		static auto initialize(entt::registry& registry) noexcept -> void;

		// 更新
		static auto render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void;
	};
} // namespace systems
