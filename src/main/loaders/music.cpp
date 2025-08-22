#include <loaders/music.hpp>

#include <SFML/Audio.hpp>

namespace loaders
{
	auto Music::operator()(const std::filesystem::path& path) noexcept -> result_type
	{
		if (not exists(path))
		{
			return nullptr;
		}

		auto music = std::make_shared<sf::Music>();
		if (not music->openFromFile(path))
		{
			return nullptr;
		}

		return music;
	}

	auto Music::operator()(const void* data, const std::size_t size) noexcept -> result_type
	{
		if (data == nullptr or size == 0)
		{
			return nullptr;
		}

		auto music = std::make_shared<sf::Music>();
		if (not music->openFromMemory(data, size))
		{
			return nullptr;
		}

		return music;
	}

	auto Music::operator()(sf::InputStream& stream) noexcept -> result_type
	{
		auto music = std::make_shared<sf::Music>();
		if (not music->openFromStream(stream))
		{
			return nullptr;
		}

		return music;
	}
}
