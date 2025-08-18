#include <texture_atlas.hpp>

#include <algorithm>
#include <ranges>

#include <sprite_region_selector.hpp>

#include <SFML/Graphics.hpp>

#include <external/imgui-SFML.hpp>

#include <imgui.h>

namespace editor
{
	auto TextureAtlas::Tile::get() const noexcept -> sf::Sprite&
	{
		auto& atlas = sprite_atlas.get();
		auto& sprite = atlas.sprites[sprite_x, sprite_y];

		return sprite;
	}

	auto TextureAtlas::SpriteAtlas::reload() noexcept -> void
	{
		assert(sprite_size > 0);
		assert(sprite_count_per_line > 0);

		const auto sprite_rect = full_sprite.getTextureRect();
		const auto sprite_rect_position = sprite_rect.position;
		const auto sprite_rect_size = sprite_rect.size;

		const auto horizontal_count = sprite_rect_size.x / sprite_size;
		const auto vertical_count = sprite_rect_size.y / sprite_size;

		// 复制完整的精灵
		sprites = utility::Matrix{horizontal_count, vertical_count, full_sprite};

		// 设置大小和位置
		for (size_type y = 0; y < vertical_count; ++y)
		{
			for (size_type x = 0; x < horizontal_count; ++x)
			{
				auto& sprite = sprites[x, y];

				const auto position = sf::Vector2u{sprite_rect_position.x + x * sprite_size, sprite_rect_position.y + y * sprite_size};
				const auto size = sf::Vector2u{sprite_size, sprite_size};

				sprite.setTextureRect({sf::Vector2i{position}, sf::Vector2i{size}});
			}
		}
	}

	auto TextureAtlas::create_sprite_atlas(sf::Sprite&& sprite) noexcept -> sprite_atlases_type::iterator
	{
		auto name = std::format("atlas {}", sprite_atlases_.size());
		SpriteAtlas atlas
		{
				.full_sprite = std::move(sprite),
				.sprites = {},
				.sprite_size = 0,
				.sprite_count_per_line = 0
		};

		return sprite_atlases_.emplace(std::move(name), std::move(atlas)).first;
	}

	TextureAtlas::TextureAtlas(TextureAtlas&& other) noexcept
		: region_selector_{std::exchange(other.region_selector_, nullptr)},
		  texture_{std::move(other.texture_)},
		  sprite_atlases_{std::exchange(other.sprite_atlases_, {})},
		  selected_sprite_atlas_iterator_{std::exchange(other.selected_sprite_atlas_iterator_, {})},
		  selected_tile_{std::exchange(other.selected_tile_, std::nullopt)}
	{
		std::ranges::for_each(
			sprite_atlases_ | std::views::values,
			[&](auto& atlas) noexcept -> void
			{
				atlas.full_sprite.setTexture(texture_);
			}
		);
	}

	auto TextureAtlas::operator=(TextureAtlas&& other) noexcept -> TextureAtlas&
	{
		if (this == &other)
		{
			return *this;
		}

		region_selector_ = std::exchange(other.region_selector_, nullptr);
		texture_ = std::move(other.texture_);

		sprite_atlases_ = std::exchange(other.sprite_atlases_, {});
		selected_sprite_atlas_iterator_ = std::exchange(selected_sprite_atlas_iterator_, {});

		selected_tile_ = std::exchange(selected_tile_, std::nullopt);

		std::ranges::for_each(
			sprite_atlases_ | std::views::values,
			[&](auto& atlas) noexcept -> void
			{
				atlas.full_sprite.setTexture(texture_);
			}
		);

		return *this;
	}

	TextureAtlas::~TextureAtlas() noexcept = default;

	TextureAtlas::TextureAtlas(const std::filesystem::path& path) noexcept
		: region_selector_{nullptr},
		  selected_sprite_atlas_iterator_{sprite_atlases_.end()},
		  selected_tile_{std::nullopt}
	{
		if (not exists(path))
		{
			return;
		}

		if (not texture_.loadFromFile(path))
		{
			return;
		}

		// 第一个图集是完整纹理的精灵
		selected_sprite_atlas_iterator_ = create_sprite_atlas(sf::Sprite{texture_});
	}

	auto TextureAtlas::loaded() const noexcept -> bool
	{
		return selected_sprite_atlas_iterator_ != sprite_atlases_.end();
	}

	auto TextureAtlas::has_selected() const noexcept -> bool
	{
		return selected_tile_.has_value();
	}

	auto TextureAtlas::get_selected() const noexcept -> std::optional<Tile>
	{
		return selected_tile_;
	}

	auto TextureAtlas::show() noexcept -> void
	{
		assert(loaded());

		ImGui::PushID(this);

		// 选择器
		if (region_selector_ and region_selector_->opened())
		{
			region_selector_->show();

			if (region_selector_->has_selected())
			{
				auto selected = region_selector_->get_selected();
				region_selector_->clear_selected();

				const auto& previous_atlas = selected_sprite_atlas_iterator_->second;
				selected_sprite_atlas_iterator_ = create_sprite_atlas(std::move(selected));

				// 继承所载入精灵的参数
				if (not previous_atlas.sprites.empty())
				{
					auto& selected_atlas = selected_sprite_atlas_iterator_->second;

					selected_atlas.sprite_size = previous_atlas.sprite_size;
					selected_atlas.sprite_count_per_line = previous_atlas.sprite_count_per_line;
					selected_atlas.reload();
				}
			}
		}

		// 所有精灵图集
		ImGui::SeparatorText("SpriteAtlas");
		{
			if (ImGui::BeginCombo("Select", selected_sprite_atlas_iterator_->first.c_str()))
			{
				for (auto it = sprite_atlases_.begin(); it != sprite_atlases_.end(); ++it)
				{
					if (const auto selected = it == selected_sprite_atlas_iterator_;
						ImGui::Selectable(it->first.c_str(), selected))
					{
						selected_sprite_atlas_iterator_ = it;
					}
				}

				ImGui::EndCombo();
			}

			// 基于当前选择图集选择子区域生成新的图集
			ImGui::SameLine();
			if (ImGui::Button("Load"))
			{
				if (
					// 还未构建选取器
					region_selector_ == nullptr or
					// 从新的精灵选取
					&region_selector_->ref_sprite() != &selected_sprite_atlas_iterator_->second.full_sprite
				)
				{
					region_selector_ = std::make_unique<SpriteRegionSelector>(selected_sprite_atlas_iterator_->second.full_sprite);
				}

				region_selector_->open();
			}
		}

		auto& selected_sprite_atlas = selected_sprite_atlas_iterator_->second;

		// 当前精灵图集参数
		ImGui::SeparatorText("AtlasSize");
		{
			int new_sprite_size = static_cast<int>(selected_sprite_atlas.sprite_size);
			int new_sprite_count_per_line = static_cast<int>(selected_sprite_atlas.sprite_count_per_line);

			const auto sprite_size_changed = ImGui::SliderInt(
				"Set Sprite Size",
				&new_sprite_size,
				10,
				100
			);
			const auto sprite_per_line_changed = ImGui::SliderInt(
				"Set Sprite Count Per Line",
				&new_sprite_count_per_line,
				1,
				100
			);

			if (sprite_size_changed)
			{
				selected_sprite_atlas.sprite_size = static_cast<size_type>(new_sprite_size);
			}
			if (sprite_per_line_changed)
			{
				selected_sprite_atlas.sprite_count_per_line = static_cast<size_type>(new_sprite_count_per_line);
			}

			if (
				(sprite_size_changed or sprite_per_line_changed) and
				new_sprite_size > 0 and
				new_sprite_count_per_line > 0
			)
			{
				selected_sprite_atlas.reload();

				if (selected_tile_.has_value() and &selected_tile_->sprite_atlas.get() == &selected_sprite_atlas)
				{
					// 更改精灵图集尺寸取消当前选择
					selected_tile_ = std::nullopt;
				}
			}
		}

		// 当前选择的子精灵
		ImGui::SeparatorText("Selected");
		if (selected_tile_.has_value())
		{
			const auto& atlas = selected_tile_->sprite_atlas.get();
			const auto& sprite = atlas.sprites[selected_tile_->sprite_x, selected_tile_->sprite_y];

			ImGui::Image(sprite);
		}
		else
		{
			ImGui::TextUnformatted("No Tile Selected");
		}

		// 子精灵按钮
		ImGui::SeparatorText("Atlas");
		if (const auto& sprites = selected_sprite_atlas.sprites;
			sprites.empty())
		{
			ImGui::TextUnformatted("Empty");
		}
		else
		{
			// 按钮间距
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
			// 按钮内边距
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0, 0});
			// 窗口内边距
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

			for (size_type y = 0; y < sprites.height(); ++y)
			{
				for (size_type x = 0; x < sprites.width(); ++x)
				{
					const auto i = static_cast<int>(y * sprites.width() + x);
					ImGui::PushID(i);

					const auto& sprite = sprites[x, y];

					if (const auto sprite_size = sf::Vector2f{sprite.getTextureRect().size};
						ImGui::ImageButton("sprite", sprite, sprite_size))
					{
						selected_tile_ = Tile
						{
								.sprite_atlas = selected_sprite_atlas,
								.sprite_x = x,
								.sprite_y = y,
						};
					}

					// 悬停效果(白色边框)
					if (ImGui::IsItemHovered())
					{
						ImGui::GetWindowDrawList()->AddRect(
							ImGui::GetItemRectMin(),
							ImGui::GetItemRectMax(),
							IM_COL32(255, 255, 255, 200)
						);
					}

					ImGui::PopID();

					if ((x + 1) % selected_sprite_atlas.sprite_count_per_line != 0)
					{
						ImGui::SameLine();
					}
				}
			}

			// 恢复样式
			ImGui::PopStyleVar(3);
		}

		ImGui::PopID();
	}
}
