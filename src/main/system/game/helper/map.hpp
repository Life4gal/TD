#pragma once

#include <entt/fwd.hpp>

namespace game::system::helper
{
	class Map
	{
	public:
		// 载入地图
		// todo: 参数应该怎么传?传文件名还是地图名?
		static auto load(entt::registry& registry) noexcept -> void;
	};
}
