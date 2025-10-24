#include <helper/asset.hpp>

#include <components/asset.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto Asset::font_of(entt::registry& registry, const entt::id_type id) noexcept -> entt::resource<const sf::Font>
	{
		using namespace components;

		const auto& [fonts] = registry.ctx().get<const asset::Fonts>();

		// todo: 如果不存在则返回默认字体
		assert(fonts.contains(id));
		const auto font = fonts[asset::constants::hud];

		return font;
	}

	auto Asset::texture_of(entt::registry& registry, const entt::id_type id) noexcept -> entt::resource<const sf::Texture>
	{
		using namespace components;

		const auto& [textures] = registry.ctx().get<const asset::Textures>();

		// todo: 如果不存在则返回默认纹理
		assert(textures.contains(id));
		const auto texture = textures[id];

		return texture;
	}

	auto Asset::sound_of(entt::registry& registry, const entt::id_type id) noexcept -> entt::resource<const loaders::SoundResource>
	{
		using namespace components;

		const auto& [sounds] = registry.ctx().get<asset::Sounds>();

		// todo: 如果不存在则返回默认音效
		assert(sounds.contains(id));
		const auto sound = sounds[id];

		return sound;
	}
}
