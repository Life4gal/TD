#include <manager/font.hpp>

namespace manager
{
	Font::~Font() noexcept = default;

	Font::Font() noexcept = default;

	auto Font::instance() noexcept -> Font&
	{
		static Font font{};
		return font;
	}

	auto Font::precache(const std::filesystem::path& path) noexcept -> std::optional<FontId>
	{
		if (not exists(path))
		{
			return std::nullopt;
		}

		if (const auto it = std::ranges::find(fonts_, path, &font_descriptor::path);
			it != fonts_.end())
		{
			const auto index = std::ranges::distance(fonts_.begin(), it);
			return static_cast<FontId>(index);
		}

		sf::Font font{};
		if (not font.openFromFile(path))
		{
			return std::nullopt;
		}

		const auto id = static_cast<FontId>(fonts_.size());
		fonts_.emplace_back(path, std::move(font));

		return id;
	}

	auto Font::reload(const FontId id, const std::filesystem::path& path) noexcept -> bool
	{
		const auto index = std::to_underlying(id);

		if (index > fonts_.size())
		{
			return false;
		}

		if (not exists(path))
		{
			return false;
		}

		auto& [cached_path, cached_font] = fonts_[index];

		if (not cached_font.openFromFile(path))
		{
			return false;
		}
		cached_path = path;

		return true;
	}

	auto Font::font_of(const FontId id) noexcept -> sf::Font&
	{
		const auto index = std::to_underlying(id);

		assert(index < fonts_.size());

		return fonts_[index].font;
	}
}
