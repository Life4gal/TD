#pragma once

#include <loaders/config.hpp>

#include <entt/core/type_traits.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/resource/cache.hpp>

namespace components
{
	namespace constants
	{
		using entt::literals::operator""_hs;

		using window_width = entt::tag<"WindowWidth"_hs>;
		using window_height = entt::tag<"WindowHeight"_hs>;
	}

	class Configs
	{
	public:
		entt::resource_cache<loaders::Config::config_type, loaders::Config> configs;
	};
}
