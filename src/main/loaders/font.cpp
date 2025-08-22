#include <loaders/font.hpp>

#include <SFML/Graphics.hpp>

namespace loaders
{
	extern const std::filesystem::path font_path;
	extern const std::filesystem::path font_extension;
}

namespace
{
	using loaders::Font;

	[[nodiscard]] auto do_load_font(std::filesystem::path filename) noexcept -> Font::result_type
	{
		// // 组合extension获得完整文件名
		// filename.replace_extension(loaders::font_extension);
		// // 组合base_path获得绝对路径
		// const auto absolute_path = loaders::font_extension / filename;

		// todo: 字体文件?
		std::ignore = filename;
		const auto absolute_path = std::filesystem::path{R"(C:\Windows\Fonts\msyh.ttc)"};

		if (not exists(absolute_path))
		{
			return nullptr;
		}

		auto font = std::make_shared<sf::Font>();
		if (not font->openFromFile(absolute_path))
		{
			return nullptr;
		}

		return font;
	}
}

namespace loaders
{
	auto Font::operator()(const std::string_view filename_without_extension) noexcept -> result_type
	{
		return do_load_font(filename_without_extension);
	}

	auto Font::operator()(const void* data, const std::size_t size) noexcept -> result_type
	{
		if (data == nullptr or size == 0)
		{
			return nullptr;
		}

		auto font = std::make_shared<sf::Font>();
		if (not font->openFromMemory(data, size))
		{
			return nullptr;
		}

		return font;
	}

	auto Font::operator()(sf::InputStream& stream) noexcept -> result_type
	{
		auto font = std::make_shared<sf::Font>();
		if (not font->openFromStream(stream))
		{
			return nullptr;
		}

		return font;
	}
}
