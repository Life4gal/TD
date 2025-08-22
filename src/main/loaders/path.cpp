#include <filesystem>

namespace loaders
{
	extern const std::filesystem::path base_path{std::filesystem::current_path()};

	extern const std::filesystem::path config_path{base_path / "config"};
	extern const std::filesystem::path data_path{base_path / "data"};
	extern const std::filesystem::path media_path{base_path / "media"};

	// config

	// font

	// texture
	extern const std::filesystem::path texture_map_path{media_path / "map"};
	extern const std::filesystem::path texture_map_extension{".png"};

	// sound

	// music
}
