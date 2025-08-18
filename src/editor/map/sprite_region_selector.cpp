#include <sprite_region_selector.hpp>

#include <SFML/Graphics.hpp>

#include <external/imgui-SFML.hpp>

#include <imgui.h>

namespace editor
{
	auto SpriteRegionSelector::select_sprite_of_selected_region() noexcept -> void
	{
		const auto [position, size] = full_sprite_.get().getTextureRect();

		// 这里的坐标要转换为纹理的坐标
		const auto top_left = sf::Vector2i
		                      {
				                      std::ranges::min(select_region_start_[0], select_region_end_[0]),
				                      std::ranges::min(select_region_start_[1], select_region_end_[1]),
		                      } + position;
		const auto right_bottom = sf::Vector2i
		                          {
				                          std::ranges::max(select_region_start_[0], select_region_end_[0]),
				                          std::ranges::max(select_region_start_[1], select_region_end_[1]),
		                          } + position;

		selected_sprite_.setTextureRect({top_left, right_bottom - top_left});
	}

	auto SpriteRegionSelector::reset_sprite() noexcept -> void
	{
		const auto rect = full_sprite_.get().getTextureRect();
		selected_sprite_.setTextureRect(rect);

		// 相对图片位置的坐标
		select_region_start_ = {0, 0};
		// 图片大小
		select_region_end_ = {rect.size.x, rect.size.y};
	}

	SpriteRegionSelector::SpriteRegionSelector(const sf::Sprite& sprite) noexcept
		: full_sprite_{sprite},
		  selected_sprite_{sprite},
		  select_region_start_{},
		  select_region_end_{},
		  open_{false},
		  selecting_region_{false},
		  selected_{false}
	{
		// 在open中重置
		// reset_sprite();
	}

	auto SpriteRegionSelector::opened() const noexcept -> bool
	{
		return open_;
	}

	auto SpriteRegionSelector::open() noexcept -> void
	{
		open_ = true;
		selected_ = false;
		selecting_region_ = false;

		reset_sprite();
	}

	auto SpriteRegionSelector::show() noexcept -> void
	{
		if (not open_)
		{
			return;
		}

		ImGui::OpenPopup("SpriteRegionSelector");
		if (ImGui::BeginPopupModal(
			"SpriteRegionSelector",
			&open_,
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_AlwaysAutoResize
		))
		{
			ImGui::SeparatorText("Original");
			ImGui::Image(full_sprite_);

			const auto mouse_position = ImGui::GetMousePos();
			const auto image_position = ImGui::GetItemRectMin();
			const auto image_size = ImGui::GetItemRectSize();

			const auto selected_sprite_rect = selected_sprite_.getTextureRect();

			// 这是选择区域在整张纹理上的位置
			ImGui::SeparatorText("RegionOnTexture");
			ImGui::Text(
				"Position: (%d:%d), Size: (%d:%d)",
				selected_sprite_rect.position.x,
				selected_sprite_rect.position.y,
				selected_sprite_rect.size.x,
				selected_sprite_rect.size.y
			);

			// 这是相对于图片显示位置(image_position)的坐标
			ImGui::SeparatorText("Select");
			{
				bool changed = false;

				changed ^= ImGui::InputInt2("Start", select_region_start_.data(), ImGuiInputTextFlags_AutoSelectAll);
				changed ^= ImGui::InputInt2("End", select_region_end_.data(), ImGuiInputTextFlags_AutoSelectAll);

				if (changed)
				{
					select_sprite_of_selected_region();
				}
			}

			if (selected_sprite_rect != full_sprite_.get().getTextureRect())
			{
				ImGui::SeparatorText("Selected");
				ImGui::Image(selected_sprite_);
			}

			auto mouse_offset_from_image = sf::Vector2i
			{
					static_cast<int>(mouse_position.x - image_position.x),
					static_cast<int>(mouse_position.y - image_position.y),
			};
			const auto is_mouse_on_image =
					mouse_offset_from_image.x >= 0 and
					mouse_offset_from_image.y >= 0 and
					mouse_offset_from_image.x < static_cast<int>(image_size.x) and
					mouse_offset_from_image.y < static_cast<int>(image_size.y);

			if (is_mouse_on_image)
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					selecting_region_ = true;

					select_region_start_ = {mouse_offset_from_image.x, mouse_offset_from_image.y};
				}
				else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					if (selecting_region_)
					{
						selecting_region_ = false;
					}
					else
					{
						reset_sprite();
					}
				}
			}

			if (selecting_region_)
			{
				// 限制鼠标在纹理区域内
				if (not is_mouse_on_image)
				{
					mouse_offset_from_image.x = std::ranges::clamp(mouse_offset_from_image.x, 0, static_cast<int>(image_size.x));
					mouse_offset_from_image.y = std::ranges::clamp(mouse_offset_from_image.y, 0, static_cast<int>(image_size.y));
				}

				select_region_end_ = {mouse_offset_from_image.x, mouse_offset_from_image.y};

				const ImVec2 start
				{
						static_cast<float>(select_region_start_[0]) + image_position.x,
						static_cast<float>(select_region_start_[1]) + image_position.y,
				};
				const ImVec2 end
				{
						static_cast<float>(select_region_end_[0]) + image_position.x,
						static_cast<float>(select_region_end_[1]) + image_position.y,
				};
				const auto start_string = std::format("({}:{})", select_region_start_[0], select_region_start_[1]);
				const auto end_string = std::format("({}:{})", select_region_end_[0], select_region_end_[1]);

				auto* draw_list = ImGui::GetWindowDrawList();

				// 选择区域方框
				draw_list->AddRect(
					start,
					end,
					IM_COL32(255, 255, 0, 255),
					0,
					0,
					2.f
				);
				// 选择起点坐标
				draw_list->AddText(
					{start.x, start.y - ImGui::GetFontSize()},
					IM_COL32(255, 255, 255, 255),
					start_string.c_str(),
					start_string.c_str() + start_string.size()
				);
				// 选择终点坐标
				draw_list->AddText(
					{end.x + 5.f, end.y - ImGui::GetFontSize()},
					IM_COL32(255, 255, 255, 255),
					end_string.c_str(),
					end_string.c_str() + end_string.size()
				);
			}

			if (selecting_region_ and ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				selecting_region_ = false;

				select_sprite_of_selected_region();
			}

			if (ImGui::Button("Confirm"))
			{
				selected_ = true;
				open_ = false;
				selecting_region_ = false;

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				selected_ = false;
				open_ = false;
				selecting_region_ = false;

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	auto SpriteRegionSelector::ref_sprite() const noexcept -> const sf::Sprite&
	{
		return full_sprite_;
	}

	auto SpriteRegionSelector::has_selected() const noexcept -> bool
	{
		return selected_;
	}

	auto SpriteRegionSelector::get_selected() const noexcept -> sf::Sprite
	{
		assert(selected_);

		return selected_sprite_;
	}

	auto SpriteRegionSelector::clear_selected() noexcept -> void
	{
		selected_ = false;

		reset_sprite();
	}
}
