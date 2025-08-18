#pragma once

#include <array>

#include <SFML/Graphics/Sprite.hpp>

namespace editor
{
	class SpriteRegionSelector
	{
	public:
		//

	private:
		// 完整精灵
		std::reference_wrapper<const sf::Sprite> full_sprite_;
		// 选择的精灵
		sf::Sprite selected_sprite_;

		// 选择区域起点(相对于图片显示位置的坐标)
		std::array<int, 2> select_region_start_;
		// 选择区域终点(相对于图片显示位置的坐标)
		std::array<int, 2> select_region_end_;

		// 窗口是否打开
		bool open_;
		// 是否正在选择区域
		bool selecting_region_;
		// 是否选择完毕
		bool selected_;

		// 按照select_region_start/end设置选择的精灵
		auto select_sprite_of_selected_region() noexcept -> void;

		// 重置选择的精灵为完整精灵
		auto reset_sprite() noexcept -> void;

	public:
		explicit SpriteRegionSelector(const sf::Sprite& sprite) noexcept;

		// 窗口是否打开
		[[nodiscard]] auto opened() const noexcept -> bool;

		// 打开窗口(关闭窗口,取消选择,确定选择都会关闭窗口,此时可以再次打开窗口继续选择新的区域)
		auto open() noexcept -> void;

		// 显示窗口
		auto show() noexcept -> void;

		// 获取当前引用的完整精灵(用于选取同一个精灵区域时避免重新构造选择器)
		[[nodiscard]] auto ref_sprite() const noexcept -> const sf::Sprite&;

		[[nodiscard]] auto has_selected() const noexcept -> bool;

		[[nodiscard]] auto get_selected() const noexcept -> sf::Sprite;

		auto clear_selected() noexcept -> void;
	};
}
