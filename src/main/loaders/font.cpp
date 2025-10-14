#include <loaders/font.hpp>

// #include <loaders/path.hpp>

#include <SFML/Graphics.hpp>

namespace loaders
{
	auto Font::operator()(const std::string_view filename_without_extension) noexcept -> result_type
	{
		// todo: 还没有准备字体资源
		// const auto absolute_path = Path::font(filename_without_extension);
		std::ignore = filename_without_extension;
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
