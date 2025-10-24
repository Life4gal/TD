#pragma once

#include <loaders/config.hpp>
#include <loaders/font.hpp>
#include <loaders/texture.hpp>
#include <loaders/sound.hpp>

#include <entt/core/hashed_string.hpp>
#include <entt/resource/cache.hpp>

namespace components::asset
{
	namespace constants
	{
		using entt::literals::operator""_hs;

		// ================
		// CONFIG

		// 窗口宽度
		constexpr auto window_width = "WindowWidth"_hs;
		// 窗口高度
		constexpr auto window_height = "WindowHeight"_hs;

		// ================
		// FONT

		// HUD字体
		constexpr auto hud = "HUD"_hs;

		// ================
		// TEXTURE

		// 地图纹理
		constexpr auto map = "Map"_hs;

		// ================
		// SOUND
	}

	class Configs
	{
	public:
		entt::resource_cache<loaders::Config::config_type, loaders::Config> configs;
	};

	class Fonts
	{
	public:
		entt::resource_cache<sf::Font, loaders::Font> fonts;
	};

	class Textures
	{
	public:
		entt::resource_cache<sf::Texture, loaders::Texture> textures;
	};

	class Sounds
	{
	public:
		entt::resource_cache<loaders::SoundResource, loaders::Sound> sounds;
	};
}
