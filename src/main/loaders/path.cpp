#include <loaders/path.hpp>

namespace
{
	[[nodiscard]] auto current_path() noexcept -> const std::filesystem::path&
	{
		static auto base_path = std::filesystem::current_path();

		return base_path;
	}
}

namespace loaders
{
	auto Path::config() noexcept -> const std::filesystem::path&
	{
		static auto path = current_path() / "config";

		return path;
	}

	auto Path::config(const std::string_view filename_without_extension) noexcept -> std::filesystem::path
	{
		std::filesystem::path path{filename_without_extension};
		path.replace_extension(".json");

		auto absolute_path = config() / path;
		return absolute_path;
	}

	auto Path::font() noexcept -> const std::filesystem::path&
	{
		static auto path = current_path() / "media" / "font";

		return path;
	}

	auto Path::font(const std::string_view filename_without_extension) noexcept -> std::filesystem::path
	{
		std::filesystem::path path{filename_without_extension};
		path.replace_extension(".ttc");

		auto absolute_path = font() / path;
		return absolute_path;
	}

	auto Path::texture() noexcept -> const std::filesystem::path&
	{
		static auto path = current_path() / "media";

		return path;
	}

	auto Path::texture_map() noexcept -> const std::filesystem::path&
	{
		static auto path = texture() / "map";

		return path;
	}

	auto Path::texture_map(const std::string_view filename_without_extension) noexcept -> std::filesystem::path
	{
		std::filesystem::path path{filename_without_extension};
		path.replace_extension(".png");

		auto absolute_path = texture_map() / path;
		return absolute_path;
	}

	auto Path::texture_enemy() noexcept -> const std::filesystem::path&
	{
		static auto path = texture() / "enemy";

		return path;
	}

	auto Path::texture_enemy(const std::string_view filename_without_extension) noexcept -> std::filesystem::path
	{
		std::filesystem::path path{filename_without_extension};
		path.replace_extension(".png");

		auto absolute_path = texture_enemy() / path;
		return absolute_path;
	}

	auto Path::texture_tower() noexcept -> const std::filesystem::path&
	{
		static auto path = texture() / "tower";

		return path;
	}

	auto Path::texture_tower(const std::string_view filename_without_extension) noexcept -> std::filesystem::path
	{
		std::filesystem::path path{filename_without_extension};
		path.replace_extension(".png");

		auto absolute_path = texture_tower() / path;
		return absolute_path;
	}

	auto Path::sound() noexcept -> const std::filesystem::path&
	{
		static auto path = current_path() / "media" / "sound";

		return path;
	}

	auto Path::sound(const std::string_view filename_without_extension) noexcept -> std::filesystem::path
	{
		std::filesystem::path path{filename_without_extension};
		path.replace_extension(".wav");

		auto absolute_path = sound() / path;
		return absolute_path;
	}

	auto Path::music() noexcept -> const std::filesystem::path&
	{
		static auto path = current_path() / "media" / "music";

		return path;
	}

	auto Path::music(const std::string_view filename_without_extension) noexcept -> std::filesystem::path
	{
		std::filesystem::path path{filename_without_extension};
		path.replace_extension(".ogg");

		auto absolute_path = music() / path;
		return absolute_path;
	}
}
