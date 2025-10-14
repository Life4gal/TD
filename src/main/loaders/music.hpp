#pragma once

#include <filesystem>
#include <memory>

namespace sf
{
	class Music;

	class InputStream;
}

namespace loaders
{
	class Music
	{
	public:
		using result_type = std::shared_ptr<sf::Music>;

		// music_name ==> media/music/music_name.ogg
		[[nodiscard]] static auto operator()(std::string_view filename_without_extension) noexcept -> result_type;

		[[nodiscard]] static auto operator()(const void* data, std::size_t size) noexcept -> result_type;

		[[nodiscard]] static auto operator()(sf::InputStream& stream) noexcept -> result_type;
	};
}
