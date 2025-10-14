#pragma once

#include <entt/fwd.hpp>

namespace sf
{
	class Font;
	class Texture;
}

namespace loaders
{
	class SoundResource;
}

namespace helper
{
	class Asset
	{
	public:
		// 预加载(当前地图)所有资源
		static auto initialize(entt::registry& registry) noexcept -> void;

		// todo: config?

		// 获取指定ID字体,保证不为空(如果不存在则返回默认字体)
		[[nodiscard]] static auto font_of(entt::registry& registry, entt::id_type id) noexcept -> entt::resource<const sf::Font>;

		// 获取指定ID纹理,保证不为空(如果不存在则返回默认纹理)
		[[nodiscard]] static auto texture_of(entt::registry& registry, entt::id_type id) noexcept -> entt::resource<const sf::Texture>;

		// 获取指定ID音效,保证不为空(如果不存在则返回默认音效)
		[[nodiscard]] static auto sound_of(entt::registry& registry, entt::id_type id) noexcept -> entt::resource<const loaders::SoundResource>;
	};
}
