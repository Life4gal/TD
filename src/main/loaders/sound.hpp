#pragma once

#include <vector>
#include <filesystem>
#include <memory>

namespace sf
{
	class SoundBuffer;
	class Sound;

	class InputStream;
}

namespace loaders
{
	class Sound
	{
	public:
		class Resource
		{
		public:
			constexpr static auto min_volume = .0f;
			constexpr static auto max_volume = 100.f;

		private:
			std::unique_ptr<sf::SoundBuffer> sound_buffer_;
			std::vector<sf::Sound> playing_sounds_;
			float current_volume_;
			float saved_volume_;

			auto remove_stopped_sound() noexcept -> void;

			auto do_play(bool pitch) noexcept -> void;

		public:
			explicit Resource(std::unique_ptr<sf::SoundBuffer> sound_buffer) noexcept;

			auto play() noexcept -> void;

			auto play_pitch_mod() noexcept -> void;

			auto set_volume(float volume) noexcept -> void;

			auto mute() noexcept -> void;

			auto un_mute(float volume) noexcept -> void;

			auto un_mute() noexcept -> void;
		};

		using result_type = std::shared_ptr<Resource>;

		[[nodiscard]] static auto operator()(const std::filesystem::path& path) noexcept -> result_type;

		[[nodiscard]] static auto operator()(const void* data, std::size_t size) noexcept -> result_type;

		[[nodiscard]] static auto operator()(sf::InputStream& stream) noexcept -> result_type;
	};
}
