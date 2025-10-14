#pragma once

#include <memory>
#include <string>

#include <SFML/Graphics/Rect.hpp>

namespace sf
{
	class Texture;

	class InputStream;
	class Image;
}

namespace loaders
{
	enum class TextureType : std::uint8_t
	{
		// media/map
		MAP,
		// media/enemy
		ENEMY,
		// media/tower
		TOWER,
	};

	class Texture
	{
	public:
		using result_type = std::shared_ptr<sf::Texture>;

		// TextureType::MAP, map_name ==> media/map/map_name.png
		// TextureType::ENEMY, enemy_name ==> media/enemy/enemy_name.png
		// TextureType::TOWER, tower_name ==> media/enemy/tower_name.png
		[[nodiscard]] static auto operator()(TextureType type, std::string_view filename_without_extension, bool s_rgb = false, const sf::IntRect& area = {}) noexcept -> result_type;

		[[nodiscard]] static auto operator()(const void* data, std::size_t size, bool s_rgb = false, const sf::IntRect& area = {}) noexcept -> result_type;

		[[nodiscard]] static auto operator()(sf::InputStream& stream, bool s_rgb = false, const sf::IntRect& area = {}) noexcept -> result_type;

		[[nodiscard]] static auto operator()(const sf::Image& image, bool s_rgb = false, const sf::IntRect& area = {}) noexcept -> result_type;
	};
}
