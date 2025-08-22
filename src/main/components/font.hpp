#pragma once

#include <loaders/font.hpp>

#include <entt/core/hashed_string.hpp>
#include <entt/resource/cache.hpp>

namespace components
{
	namespace constants
	{
		using entt::literals::operator""_hs;

		// HUD字体
		constexpr auto hud = "HUD"_hs;
	}

	class Fonts
	{
	public:
		entt::resource_cache<sf::Font, loaders::Font> fonts;
	};
}
