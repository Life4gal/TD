#pragma once

#include <memory>
#include <string>

namespace sf
{
	class Font;

	class InputStream;
}

namespace loaders
{
	class Font
	{
	public:
		using result_type = std::shared_ptr<sf::Font>;

		// font_name ==> media/font/font_name.ttf
		[[nodiscard]] static auto operator()(std::string_view filename_without_extension) noexcept -> result_type;

		[[nodiscard]] static auto operator()(const void* data, std::size_t size) noexcept -> result_type;

		[[nodiscard]] static auto operator()(sf::InputStream& stream) noexcept -> result_type;
	};
}
