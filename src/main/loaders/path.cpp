#include <filesystem>

namespace loaders
{
	extern const std::filesystem::path base_path{std::filesystem::current_path()};

	extern const std::filesystem::path config_path{base_path / "config"};
	extern const std::filesystem::path data_path{base_path / "data"};
	extern const std::filesystem::path media_path{base_path / "media"};

	namespace config
	{
		//
	}

	namespace font
	{
		extern const std::filesystem::path path{media_path / "font"};
		extern const std::filesystem::path extension{".ttc"};
	} // namespace font

	namespace texture
	{
		extern const std::filesystem::path map_path{media_path / "map"};
		extern const std::filesystem::path map_extension{".png"};

		extern const std::filesystem::path enemy_path{media_path / "enemy"};
		extern const std::filesystem::path enemy_extension{".png"};

		extern const std::filesystem::path tower_path{media_path / "tower"};
		extern const std::filesystem::path tower_extension{".png"};
	} // namespace texture

	namespace sound
	{
		//
	}

	namespace music
	{
		//
	}
}
