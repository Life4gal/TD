#pragma once

#include <unordered_map>

#include <texture_atlas.hpp>

#include <external/imgui-file_browser.hpp>

namespace sf
{
	class RenderWindow;
}

namespace editor
{
	class Map
	{
	public:
		using size_type = TextureAtlas::size_type;

	private:
		// 路径 <-> 文件名+纹理图集
		using texture_atlases_type = std::unordered_map<std::filesystem::path, std::pair<std::string, TextureAtlas>>;

		// 文件浏览器,用于载入新的纹理文件
		ImGui::FileBrowser file_browser_;

		// 图片保存的路径
		ImGui::FileBrowser::edit_string_buffer_type save_image_filename_;
		std::filesystem::path save_image_path_;

		// 载入的纹理图集
		texture_atlases_type texture_atlases_;
		// 当前选择的纹理图集
		texture_atlases_type::iterator selected_texture_atlas_iterator_;

		// 用于显示的地图(也用于输出图片)
		utility::Matrix<std::optional<TextureAtlas::Tile>> tile_map_;
		// 地图网格大小(如果于纹理图集中的精灵大小不一致则需要进行缩放)
		size_type tile_size_;

		// 基于新的width/height构造TileMap,复制之前的数据
		auto reload_tile_map(std::uint32_t width, std::uint32_t height) noexcept -> void;

		// 将当前的TileMap保存到图片
		auto save_image(const std::filesystem::path& path) const noexcept -> void;

	public:
		Map() noexcept;

		// 将选择的精灵应用到地图上(如果未选取任何精灵则什么也不做)
		auto place_tile_map(sf::Vector2u position) noexcept -> void;

		// 显示(imgui)窗口
		auto show() noexcept -> void;

		// 将内容渲染到窗口中
		auto render(sf::RenderWindow& window) noexcept -> void;
	};
}
