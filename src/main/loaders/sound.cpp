#include <loaders/sound.hpp>

#include <algorithm>
#include <ranges>
#include <random>

#include <loaders/path.hpp>

#include <SFML/Audio.hpp>

namespace loaders
{
	auto SoundResource::remove_stopped_sound() const noexcept -> void
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

	auto SoundResource::do_play(const bool pitch) const noexcept -> void
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

	SoundResource::SoundResource(std::unique_ptr<sf::SoundBuffer> sound_buffer) noexcept
		: sound_buffer_{std::move(sound_buffer)},
		  current_volume_{max_volume},
		  saved_volume_{max_volume} {}

	auto SoundResource::play() const noexcept -> void
	{
		do_play(false);
	}

	auto SoundResource::play_pitch_mod() const noexcept -> void
	{
		do_play(true);
	}

	auto SoundResource::set_volume(const float volume) noexcept -> void
	{
		current_volume_ = std::ranges::clamp(volume, min_volume, max_volume);
		saved_volume_ = current_volume_;

		std::ranges::for_each(
			playing_sounds_,
			std::bind_back(&sf::Sound::setVolume, current_volume_)
		);
	}

	auto SoundResource::mute() noexcept -> void
	{
		saved_volume_ = current_volume_;
		current_volume_ = min_volume;

		std::ranges::for_each(
			playing_sounds_,
			std::bind_back(&sf::Sound::setVolume, current_volume_)
		);
	}

	auto SoundResource::un_mute(const float volume) noexcept -> void
	{
		current_volume_ = std::ranges::clamp(volume, min_volume, max_volume);

		std::ranges::for_each(
			playing_sounds_,
			std::bind_back(&sf::Sound::setVolume, current_volume_)
		);
	}

	auto SoundResource::un_mute() noexcept -> void
	{
		un_mute(saved_volume_);
	}

	auto Sound::operator()(const std::string_view filename_without_extension) noexcept -> result_type
	{
		const auto absolute_path = Path::sound(filename_without_extension);

		if (not exists(absolute_path))
		{
			return nullptr;
		}

		auto sound_buffer = std::make_unique<sf::SoundBuffer>();
		if (not sound_buffer->loadFromFile(absolute_path))
		{
			return nullptr;
		}

		return std::make_shared<SoundResource>(std::move(sound_buffer));
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

		return std::make_shared<SoundResource>(std::move(sound_buffer));
	}

	auto Sound::operator()(sf::InputStream& stream) noexcept -> result_type
	{
		auto sound_buffer = std::make_unique<sf::SoundBuffer>();
		if (not sound_buffer->loadFromStream(stream))
		{
			return nullptr;
		}

		return std::make_shared<SoundResource>(std::move(sound_buffer));
	}
}
