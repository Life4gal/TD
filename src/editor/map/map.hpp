#pragma once

#include <unordered_map>

#include <texture_atlas.hpp>
#include <scene/scene.hpp>

#include <external/imgui-file_browser.hpp>

#include <SFML/Graphics/RenderTexture.hpp>

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

		class Layers
		{
		public:
			using element_type = std::optional<TextureAtlas::Tile>;
			using size_type = std::uint32_t;

			using layer_type = utility::MatrixView<element_type>;
			using const_layer_type = utility::MatrixView<const element_type>;

			constexpr static auto unselected = std::numeric_limits<size_type>::max();

			// 所有元素
			std::unique_ptr<element_type[]> elements;
			// 图层宽度
			size_type width;
			// 图层高度
			size_type height;

			// 所有图层
			std::unique_ptr<layer_type[]> layers;
			// 图层数量
			size_type layers_count;

			Layers() noexcept;

			[[nodiscard]] auto operator[](size_type index) noexcept -> layer_type;

			[[nodiscard]] auto operator[](size_type index) const noexcept -> const_layer_type;

			// 构建新的layer,元素设置为nullopt
			auto build(size_type init_width, size_type init_height) noexcept -> void;

			// 基于新的width/height构造layer,复制之前的数据
			auto rebuild(size_type new_width, size_type new_height) noexcept -> void;

			// 追加一个layer
			auto append_layer(size_type count) noexcept -> void;

			// 将一个图层绘制到纹理上
			static auto save_layer(sf::RenderTexture& target, Map::size_type tile_width, Map::size_type tile_height, const layer_type& layer) noexcept -> void;

			// 将所有图层绘制到纹理上
			auto save_all_layers(sf::RenderTexture& target, Map::size_type tile_width, Map::size_type tile_height) const noexcept -> void;
		};

		// 文件浏览器,用于载入新的纹理文件
		ImGui::FileBrowser file_browser_;

		// 图片保存的路径
		ImGui::FileBrowser::edit_string_buffer_type save_image_filename_;
		std::filesystem::path save_image_path_;

		// 载入的纹理图集
		texture_atlases_type texture_atlases_;
		// 当前选择的纹理图集
		texture_atlases_type::iterator selected_texture_atlas_iterator_;

		// 所有图层
		Layers layers_;
		// 当前选择的图层
		Layers::size_type selected_layers_;

		// 地图网格大小(如果于纹理图集中的精灵大小不一致则需要进行缩放)
		size_type tile_width_;
		size_type tile_height_;

		// 用于显示当前渲染层的纹理
		sf::RenderTexture display_texture_;

		// 显示纹理是否被修改
		bool display_texture_dirty_;

		// dock布局是否初始化
		bool dock_layout_initialized_;

		// 将所有图层绘制到纹理上并保存到图片
		auto save_image(const std::filesystem::path& path) const noexcept -> void;

		auto dock_setup() noexcept -> void;
		auto dock_main_window() noexcept -> void;
		auto dock_tilemap_window() noexcept -> void;
		auto dock_texture_atlas_window() noexcept -> void;
		auto dock_sprite_atlas_window() noexcept -> void;
		auto dock_console_window() const noexcept -> void;

	public:
		Map() noexcept;

		auto setup() noexcept -> void;

		auto update() noexcept -> void;
	};
}
