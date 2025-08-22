#pragma once

#include <loaders/texture.hpp>

#include <entt/core/hashed_string.hpp>
#include <entt/resource/cache.hpp>

namespace components
{
	namespace constants
	{
		using entt::literals::operator""_hs;

		constexpr auto map = "Map"_hs;
	}

	class Textures
	{
	public:
		entt::resource_cache<sf::Texture, loaders::Texture> textures;
	};
}
