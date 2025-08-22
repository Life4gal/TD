#include <loaders/sound.hpp>

#include <algorithm>
#include <ranges>
#include <random>

#include <SFML/Audio.hpp>

namespace loaders
{
	auto Sound::Resource::remove_stopped_sound() noexcept -> void
	{
		const auto ranges = std::ranges::remove_if(
			playing_sounds_,
			std::bind_back(std::ranges::equal_to{}, sf::Sound::Status::Stopped),
			&sf::Sound::getStatus
		);

		// 重分配
		// playing_sounds_ =
		// 		std::move(playing_sounds_) |
		// 		std::views::filter(
		// 			[](const auto& sound) noexcept
		// 			{
		// 				return sound.getStatus() == sf::Sound::Status::Stopped;
		// 			}
		// 		) |
		// 		std::ranges::to<std::vector>();

		// 不重分配
		playing_sounds_.erase(ranges.begin(), ranges.end());
	}

	auto Sound::Resource::do_play(const bool pitch) noexcept -> void
	{
		remove_stopped_sound();

		if (current_volume_ == min_volume) // NOLINT(clang-diagnostic-float-equal)
		{
			return;
		}

		sf::Sound new_sound{*sound_buffer_};
		new_sound.setVolume(current_volume_);
		if (pitch)
		{
			static std::mt19937 random{std::random_device{}()};
			std::uniform_real_distribution<float> distribution{0, 50};

			const auto pitch_value = (75.f + distribution(random)) / 100.f;
			new_sound.setPitch(pitch_value);
		}

		auto& sound = playing_sounds_.emplace_back(std::move(new_sound));
		sound.play();
	}

	Sound::Resource::Resource(std::unique_ptr<sf::SoundBuffer> sound_buffer) noexcept
		: sound_buffer_{std::move(sound_buffer)},
		  current_volume_{max_volume},
		  saved_volume_{max_volume} {}

	auto Sound::Resource::play() noexcept -> void
	{
		do_play(false);
	}

	auto Sound::Resource::play_pitch_mod() noexcept -> void
	{
		do_play(true);
	}

	auto Sound::Resource::set_volume(const float volume) noexcept -> void
	{
		current_volume_ = std::ranges::clamp(volume, min_volume, max_volume);
		saved_volume_ = current_volume_;

		std::ranges::for_each(
			playing_sounds_,
			std::bind_back(&sf::Sound::setVolume, current_volume_)
		);
	}

	auto Sound::Resource::mute() noexcept -> void
	{
		saved_volume_ = current_volume_;
		current_volume_ = min_volume;

		std::ranges::for_each(
			playing_sounds_,
			std::bind_back(&sf::Sound::setVolume, current_volume_)
		);
	}

	auto Sound::Resource::un_mute(const float volume) noexcept -> void
	{
		current_volume_ = std::ranges::clamp(volume, min_volume, max_volume);

		std::ranges::for_each(
			playing_sounds_,
			std::bind_back(&sf::Sound::setVolume, current_volume_)
		);
	}

	auto Sound::Resource::un_mute() noexcept -> void
	{
		un_mute(saved_volume_);
	}

	auto Sound::operator()(const std::filesystem::path& path) noexcept -> result_type
	{
		if (not exists(path))
		{
			return nullptr;
		}

		auto sound_buffer = std::make_unique<sf::SoundBuffer>();
		if (not sound_buffer->loadFromFile(path))
		{
			return nullptr;
		}

		return std::make_shared<Resource>(std::move(sound_buffer));
	}

	auto Sound::operator()(const void* data, const std::size_t size) noexcept -> result_type
	{
		if (data == nullptr || size == 0)
		{
			return nullptr;
		}

		auto sound_buffer = std::make_unique<sf::SoundBuffer>();
		if (not sound_buffer->loadFromMemory(data, size))
		{
			return nullptr;
		}

		return std::make_shared<Resource>(std::move(sound_buffer));
	}

	auto Sound::operator()(sf::InputStream& stream) noexcept -> result_type
	{
		auto sound_buffer = std::make_unique<sf::SoundBuffer>();
		if (not sound_buffer->loadFromStream(stream))
		{
			return nullptr;
		}

		return std::make_shared<Resource>(std::move(sound_buffer));
	}
}
