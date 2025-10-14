#include <loaders/texture.hpp>

#include <loaders/path.hpp>

#include <SFML/Graphics.hpp>

namespace loaders
{
	auto Texture::operator()(const TextureType type, const std::string_view filename_without_extension, const bool s_rgb, const sf::IntRect& area) noexcept -> result_type
	{
		const auto absolute_path = [&]
		{
			switch (type)
			{
				case TextureType::MAP:
				{
					return Path::texture_map(filename_without_extension);
				}
				case TextureType::ENEMY:
				{
					return Path::texture_enemy(filename_without_extension);
				}
				case TextureType::TOWER:
				{
					return Path::texture_tower(filename_without_extension);
				}
				default: // NOLINT(clang-diagnostic-covered-switch-default)
				{
					std::unreachable();
				}
			}
		}();

		if (not exists(absolute_path))
		{
			return nullptr;
		}

		auto texture = std::make_shared<sf::Texture>();
		if (not texture->loadFromFile(absolute_path, s_rgb, area))
		{
			return nullptr;
		}
		return texture;
	}

	auto Texture::operator()(const void* data, const std::size_t size, const bool s_rgb, const sf::IntRect& area) noexcept -> result_type
	{
		if (data == nullptr or size == 0)
		{
			return nullptr;
		}

		auto texture = std::make_shared<sf::Texture>();
		if (not texture->loadFromMemory(data, size, s_rgb, area))
		{
			return nullptr;
		}

		return texture;
	}

	auto Texture::operator()(sf::InputStream& stream, const bool s_rgb, const sf::IntRect& area) noexcept -> result_type
	{
		auto texture = std::make_shared<sf::Texture>();
		if (not texture->loadFromStream(stream, s_rgb, area))
		{
			return nullptr;
		}

		return texture;
	}

	auto Texture::operator()(const sf::Image& image, const bool s_rgb, const sf::IntRect& area) noexcept -> result_type
	{
		auto texture = std::make_shared<sf::Texture>();
		if (not texture->loadFromImage(image, s_rgb, area))
		{
			return nullptr;
		}

		return texture;
	}
}
