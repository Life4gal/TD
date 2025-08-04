#pragma once

#include <entt/entity/registry.hpp>

namespace global::component::registry
{
	class Registry
	{
	public:
		std::shared_ptr<entt::registry> registry;
	};
}
