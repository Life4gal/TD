#pragma once

#include <memory>
#include <filesystem>

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

		// TextureType::MAP, map1(不含后缀) ==> media/map/map1.map
		[[nodiscard]] static auto operator()(TextureType type, std::string_view filename_without_extension) noexcept -> result_type;

		[[nodiscard]] static auto operator()(const void* data, std::size_t size, bool s_rgb = false, const sf::IntRect& area = {}) noexcept -> result_type;

		[[nodiscard]] static auto operator()(sf::InputStream& stream, bool s_rgb = false, const sf::IntRect& area = {}) noexcept -> result_type;

		[[nodiscard]] static auto operator()(const sf::Image& image, bool s_rgb = false, const sf::IntRect& area = {}) noexcept -> result_type;
	};
}
