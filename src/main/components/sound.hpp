#pragma once

#include <loaders/sound.hpp>

#include <entt/core/type_traits.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/resource/cache.hpp>

namespace components
{
	namespace constants
	{
		using entt::literals::operator""_hs;

		//
	}

	class Sounds
	{
	public:
		entt::resource_cache<loaders::Sound::Resource, loaders::Sound> sounds;
	};
}
