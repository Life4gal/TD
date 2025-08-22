#include <loaders/texture.hpp>

#include <SFML/Graphics.hpp>

namespace loaders
{
	extern const std::filesystem::path texture_map_path;
	extern const std::filesystem::path texture_map_extension;
}

namespace
{
	using loaders::Texture;

	[[nodiscard]] auto do_load_map(std::filesystem::path filename) noexcept -> Texture::result_type
	{
		// 组合extension获得完整文件名
		filename.replace_extension(loaders::texture_map_extension);
		// 组合base_path获得绝对路径
		const auto absolute_path = loaders::texture_map_path / filename;

		if (not exists(absolute_path))
		{
			return nullptr;
		}

		auto texture = std::make_shared<sf::Texture>();
		if (not texture->loadFromFile(absolute_path))
		{
			return nullptr;
		}

		return texture;
	}
}

namespace loaders
{
	auto Texture::operator()(const TextureType type, const std::string_view filename_without_extension) noexcept -> result_type
	{
		switch (type)
		{
			case TextureType::MAP:
			{
				return do_load_map(filename_without_extension);
			}
			default: // NOLINT(clang-diagnostic-covered-switch-default)
			{
				std::unreachable();
			}
		}
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
