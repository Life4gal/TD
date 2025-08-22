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

		[[nodiscard]] static auto operator()(const std::filesystem::path& path) noexcept -> result_type;

		[[nodiscard]] static auto operator()(const void* data, std::size_t size) noexcept -> result_type;

		[[nodiscard]] static auto operator()(sf::InputStream& stream) noexcept -> result_type;
	};
}
