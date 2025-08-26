#pragma once

#include <memory>
#include <unordered_map>
#include <optional>

#include <utility/matrix.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace editor
{
	class SpriteRegionSelector;

	class TextureAtlas
	{
		class SpriteAtlas;

	public:
		using size_type = utility::Matrix<sf::Sprite>::size_type;

		// 选取的精灵
		class Tile
		{
		public:
			// 直接引用精灵图集
			std::reference_wrapper<SpriteAtlas> sprite_atlas;
			// 不直接引用精灵,而是储存其坐标,因为精灵可能因为图集重载而不可用
			size_type sprite_x;
			size_type sprite_y;

			[[nodiscard]] auto get() const noexcept -> sf::Sprite&;

			[[nodiscard]] auto operator==(const Tile& other) const noexcept -> bool;
		};

	private:
		class SpriteAtlas
		{
		public:
			// 完整的精灵
			sf::Sprite full_sprite;
			// 用于选取的子精灵
			utility::Matrix<sf::Sprite> sprites;
			// 每个子精灵大小
			size_type sprite_size;
			// 一行有多少个精灵
			size_type sprite_count_per_line;

			// 重载当前精灵图集子精灵
			auto reload() noexcept -> void;
		};

		// 图集名 <-> 图集
		using sprite_atlases_type = std::unordered_map<std::string, SpriteAtlas>;

		// 区域选择器
		std::unique_ptr<SpriteRegionSelector> region_selector_;

		// 纹理
		sf::Texture texture_;

		// 所有精灵
		// 如果一张纹理就是完整的精灵则只有一个图集
		// 如果一张纹理分为多个区域分别选取则会有多个图集
		sprite_atlases_type sprite_atlases_;

		// 当前选择的图集
		sprite_atlases_type::iterator selected_sprite_atlas_iterator_;

		// 精灵按钮的放大倍数(如果纹理上的尺寸较小时可以将其放大)
		sf::Vector2f tile_scale_;

		// 当前选择的子精灵
		std::optional<Tile> selected_tile_;

		// 创建一个新的精灵图集
		auto create_sprite_atlas(sf::Sprite&& sprite) noexcept -> sprite_atlases_type::iterator;

	public:
		TextureAtlas(const TextureAtlas&) noexcept = delete;
		auto operator=(const TextureAtlas&) noexcept -> TextureAtlas& = delete;

		TextureAtlas(TextureAtlas&& other) noexcept;
		auto operator=(TextureAtlas&& other) noexcept -> TextureAtlas&;

		~TextureAtlas() noexcept;

		explicit TextureAtlas(const std::filesystem::path& path) noexcept;

		// 纹理是否载入成功
		[[nodiscard]] auto loaded() const noexcept -> bool;

		// 是否有选取任何精灵
		[[nodiscard]] auto has_selected() const noexcept -> bool;

		// 获取当前选取的精灵(可能还未选取)
		[[nodiscard]] auto get_selected() const noexcept -> std::optional<Tile>;

		auto show() noexcept -> void;
	};
}
