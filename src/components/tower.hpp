#pragma once

#include <vector>

#include <entt/entity/entity.hpp>

namespace components
{
	class Modifier
	{
	public:
		// todo: 不同的塔提供不同的增益效果
		int none;
	};

	class Equipment
	{
	public:
		// 一个塔可以有多个武器
		std::vector<entt::entity> weapons;
	};
}
