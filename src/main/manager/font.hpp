#pragma once

#include <optional>

#include <SFML/Graphics/Font.hpp>

namespace manager
{
	class Font
	{
	public:
		// id (index)
		enum class FontId : std::uint32_t {};

	private:
		struct font_descriptor
		{
			std::filesystem::path path;
			sf::Font font;
		};

		std::vector<font_descriptor> fonts_;

	public:
		Font(const Font&) noexcept = delete;
		Font(Font&&) noexcept = delete;
		auto operator=(const Font&) noexcept -> Font& = delete;
		auto operator=(Font&&) noexcept -> Font& = delete;

		~Font() noexcept;

	private:
		Font() noexcept;

	public:
		[[nodiscard]] static auto instance() noexcept -> Font&;

		[[nodiscard]] auto precache(const std::filesystem::path& path) noexcept -> std::optional<FontId>;

		[[nodiscard]] auto reload(FontId id, const std::filesystem::path& path) noexcept -> bool;

		[[nodiscard]] auto font_of(FontId id) noexcept -> sf::Font&;
	};
}
