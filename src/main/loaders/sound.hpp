#pragma once

#include <vector>
#include <string>
#include <memory>

namespace sf
{
	class SoundBuffer;
	class Sound;

	class InputStream;
}

namespace loaders
{
	class SoundResource
	{
	public:
		constexpr static auto min_volume = .0f;
		constexpr static auto max_volume = 100.f;

	private:
		mutable std::unique_ptr<sf::SoundBuffer> sound_buffer_;
		mutable std::vector<sf::Sound> playing_sounds_;

		float current_volume_;
		float saved_volume_;

		auto remove_stopped_sound() const noexcept -> void;

		auto do_play(bool pitch) const noexcept -> void;

	public:
		explicit SoundResource(std::unique_ptr<sf::SoundBuffer> sound_buffer) noexcept;

		auto play() const noexcept -> void;

		auto play_pitch_mod() const noexcept -> void;

		auto set_volume(float volume) noexcept -> void;

		auto mute() noexcept -> void;

		auto un_mute(float volume) noexcept -> void;

		auto un_mute() noexcept -> void;
	};

	class Sound
	{
	public:
		using result_type = std::shared_ptr<SoundResource>;

		// sound_name .wav ==> media/sound/sound_name.wav
		[[nodiscard]] static auto operator()(std::string_view filename_without_extension) noexcept -> result_type;

		[[nodiscard]] static auto operator()(const void* data, std::size_t size) noexcept -> result_type;

		[[nodiscard]] static auto operator()(sf::InputStream& stream) noexcept -> result_type;
	};
}
