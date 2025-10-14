#pragma once

#include <filesystem>

namespace loaders
{
	class Path
	{
	public:
		// 配置文件目录绝对路径
		[[nodiscard]] static auto config() noexcept -> const std::filesystem::path&;

		// 指定配置文件绝对路径
		[[nodiscard]] static auto config(std::string_view filename_without_extension) noexcept -> std::filesystem::path;

		// 字体文件目录绝对路径
		[[nodiscard]] static auto font() noexcept -> const std::filesystem::path&;

		// 指定字体文件绝对路径
		[[nodiscard]] static auto font(std::string_view filename_without_extension) noexcept -> std::filesystem::path;

		// 纹理文件目录绝对路径
		[[nodiscard]] static auto texture() noexcept -> const std::filesystem::path&;

		// 地图纹理文件目录绝对路径
		[[nodiscard]] static auto texture_map() noexcept -> const std::filesystem::path&;

		// 指定地图纹理文件绝对路径
		[[nodiscard]] static auto texture_map(std::string_view filename_without_extension) noexcept -> std::filesystem::path;

		// 敌人纹理文件目录绝对路径
		[[nodiscard]] static auto texture_enemy() noexcept -> const std::filesystem::path&;

		// 指定敌人纹理文件绝对路径
		[[nodiscard]] static auto texture_enemy(std::string_view filename_without_extension) noexcept -> std::filesystem::path;

		// 塔纹理文件目录绝对路径
		[[nodiscard]] static auto texture_tower() noexcept -> const std::filesystem::path&;

		// 指定塔纹理文件绝对路径
		[[nodiscard]] static auto texture_tower(std::string_view filename_without_extension) noexcept -> std::filesystem::path;

		// 音效文件目录绝对路径
		[[nodiscard]] static auto sound() noexcept -> const std::filesystem::path&;

		// 指定音效文件绝对路径
		[[nodiscard]] static auto sound(std::string_view filename_without_extension) noexcept -> std::filesystem::path;

		// 背景音乐文件目录绝对路径
		[[nodiscard]] static auto music() noexcept -> const std::filesystem::path&;

		// 指定背景音乐文件绝对路径
		[[nodiscard]] static auto music(std::string_view filename_without_extension) noexcept -> std::filesystem::path;
	};
}
