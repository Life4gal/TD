#include <map.hpp>

#include <SFML/Graphics.hpp>

#include <external/imgui-SFML.hpp>

#include <imgui.h>

namespace
{
	constexpr std::string_view default_save_path{"save.png"};

	auto expand_string_buffer(ImGuiInputTextCallbackData* callback_data) noexcept -> int
	{
		if (callback_data and callback_data->EventFlag & ImGuiInputTextFlags_CallbackResize)
		{
			if (auto* string = static_cast<ImGui::FileBrowser::edit_string_buffer_type*>(callback_data->UserData);
				std::cmp_less(string->capacity, callback_data->BufSize))
			{
				const auto old_size = string->capacity;
				const auto buf_size = static_cast<std::size_t>(callback_data->BufSize);
				const auto new_size = static_cast<std::size_t>(static_cast<float>(std::ranges::max(old_size, buf_size)) * 1.5f);

				const auto old_data = std::move(string->data);
				string->data = std::make_unique_for_overwrite<char[]>(new_size);
				std::ranges::copy(old_data.get(), old_data.get() + old_size, string->data.get());
				string->data[old_size] = '\0';

				callback_data->Buf = string->data.get();
				callback_data->BufDirty = true;
			}
		}

		return 0;
	}
}

namespace editor
{
	auto Map::reload_tile_map(const std::uint32_t width, const std::uint32_t height) noexcept -> void
	{
		const auto old_tile_map = std::exchange(
			tile_map_,
			utility::Matrix<std::optional<TextureAtlas::Tile>>{width, height, std::nullopt}
		);

		tile_map_.copy_from(old_tile_map);
	}

	auto Map::save_image(const std::filesystem::path& path) const noexcept -> void
	{
		const auto map_width = tile_map_.width() * tile_size_;
		const auto map_height = tile_map_.height() * tile_size_;
		sf::RenderTexture render_texture{{map_width, map_height}};

		render_texture.clear(sf::Color::Transparent);

		// 绘制所有非空瓦片
		for (std::uint32_t y = 0; y < tile_map_.height(); ++y)
		{
			for (std::uint32_t x = 0; x < tile_map_.width(); ++x)
			{
				const auto tile = tile_map_[x, y];
				if (not tile.has_value())
				{
					continue;
				}

				auto& sprite = tile->get();

				const auto sprite_size = sprite.getTextureRect().size;

				const auto position = sf::Vector2f
				{
						static_cast<float>(tile_size_ * x),
						static_cast<float>(tile_size_ * y),
				};
				const auto scale = sf::Vector2f
				{
						static_cast<float>(tile_size_) / static_cast<float>(sprite_size.x),
						static_cast<float>(tile_size_) / static_cast<float>(sprite_size.y),
				};

				sprite.setPosition(position);
				sprite.setScale(scale);

				render_texture.draw(sprite);
			}
		}

		render_texture.display();

		const auto image = render_texture.getTexture().copyToImage();
		const auto saved = image.saveToFile(path);
		// todo: 保存失败处理?
		std::ignore = saved;
	}

	Map::Map() noexcept
		: file_browser_{"MapEditor"},
		  save_image_filename_{.data = nullptr, .capacity = 0},
		  selected_texture_atlas_iterator_{texture_atlases_.end()},
		  tile_size_{0}

	{
		file_browser_.set_filter({".jpg", ".jpeg", ".png"});

		save_image_filename_ =
		{
				.data = std::make_unique_for_overwrite<char[]>(64),
				.capacity = 64,
		};
		save_image_filename_.data[0] = '\0';

		// 创建 20x20 的瓦片地图,每个瓦片大小为32x32像素
		tile_map_ = utility::Matrix<std::optional<TextureAtlas::Tile>>{20, 20, std::nullopt};
		tile_size_ = 32;
	}

	auto Map::place_tile_map(const sf::Vector2u position) noexcept -> void
	{
		if (selected_texture_atlas_iterator_ == texture_atlases_.end())
		{
			return;
		}

		const auto& selected_texture_atlas = selected_texture_atlas_iterator_->second.second;
		if (not selected_texture_atlas.has_selected())
		{
			return;
		}

		// 转换为瓦片坐标
		const auto tile_x = position.x / tile_size_;
		const auto tile_y = position.y / tile_size_;

		// 确保在有效范围内
		if (tile_x < tile_map_.width() && tile_y < tile_map_.height())
		{
			// 放置选中的子纹理
			tile_map_[tile_x, tile_y] = selected_texture_atlas.get_selected();
		}
	}

	auto Map::show() noexcept -> void
	{
		ImGui::PushID(this);

		// 文件选择器
		{
			file_browser_.show();

			if (file_browser_.has_selected())
			{
				auto selected = file_browser_.get_selected();
				file_browser_.clear_selected();

				// 载入纹理图集
				if (TextureAtlas atlas{selected};
					atlas.loaded())
				{
					auto filename = selected.filename().string();
					selected_texture_atlas_iterator_ = texture_atlases_.emplace(std::move(selected), std::make_pair(std::move(filename), std::move(atlas))).first;
				}
				else
				{
					// todo: 载入纹理失败后怎么做?
				}
			}
		}

		// const auto pos = ImGui::GetIO().MousePos;
		// const auto tile_x = static_cast<size_type>(pos.x) / tile_size_;
		// const auto tile_y = static_cast<size_type>(pos.y) / tile_size_;
		// ImGui::Text("(%.0f:%.0f)[%u:%u]", pos.x, pos.y, tile_x, tile_y);

		ImGui::SeparatorText("TileMap");
		{
			// 设置参数
			{
				int width = static_cast<int>(tile_map_.width());
				int height = static_cast<int>(tile_map_.height());
				int tile_size = static_cast<int>(tile_size_);

				const auto width_changed = ImGui::SliderInt("Set TileMap Width", &width, 1, 100);
				const auto height_changed = ImGui::SliderInt("Set TileMap Height", &height, 1, 100);
				const auto tile_size_changed = ImGui::SliderInt("Set Tile Size", &tile_size, 1, 64);

				if (width_changed or height_changed)
				{
					reload_tile_map(width, height);
				}
				if (tile_size_changed)
				{
					tile_size_ = static_cast<size_type>(tile_size);
				}
			}

			ImGui::InputText(
				"Save Image Filename",
				save_image_filename_.data.get(),
				save_image_filename_.capacity,
				ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_AutoSelectAll,
				expand_string_buffer,
				&save_image_filename_
			);

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				const auto length = std::strlen(save_image_filename_.data.get());
				save_image_filename_.data[length] = '\0';

				if (length == 0)
				{
					std::ranges::copy(default_save_path, save_image_filename_.data.get());
					save_image_filename_.data[default_save_path.size()] = '\0';
				}
			}

			// 保存
			{
				static const char* types[]{"png", "jpg"};
				static int selected_type = 0;

				if (ImGui::BeginCombo("Type", types[selected_type]))
				{
					for (int i = 0; i < 2; ++i)
					{
						if (const auto selected = i == selected_type;
							ImGui::Selectable(types[i], selected))
						{
							selected_type = i;
						}
					}

					ImGui::EndCombo();
				}
				ImGui::SameLine();
				if (ImGui::Button("Save"))
				{
					const auto full_filename = std::format("{}.{}", save_image_filename_.data.get(), types[selected_type]);
					const auto path = std::filesystem::current_path() / full_filename;

					save_image(path);
				}
			}
		}

		ImGui::SeparatorText("TextureAtlas");
		{
			// 载入新的纹理图集
			if (ImGui::Button("Add"))
			{
				file_browser_.open();
			}

			// 显示当前纹理图集
			if (texture_atlases_.empty())
			{
				ImGui::TextUnformatted("No Texture Loaded");
			}
			else
			{
				if (ImGui::BeginCombo("Select", selected_texture_atlas_iterator_->second.first.c_str()))
				{
					for (auto it = texture_atlases_.begin(); it != texture_atlases_.end(); ++it)
					{
						if (const auto selected = it == selected_texture_atlas_iterator_;
							ImGui::Selectable(it->second.first.c_str(), selected))
						{
							selected_texture_atlas_iterator_ = it;
						}
					}

					ImGui::EndCombo();
				}

				selected_texture_atlas_iterator_->second.second.show();
			}
		}

		ImGui::PopID();
	}

	auto Map::render(sf::RenderWindow& window) noexcept -> void
	{
		for (size_type y = 0; y < tile_map_.height(); ++y)
		{
			for (size_type x = 0; x < tile_map_.width(); ++x)
			{
				if (const auto tile = tile_map_[x, y];
					tile.has_value())
				{
					auto& sprite = tile->get();

					const auto sprite_size = sprite.getTextureRect().size;

					const auto position = sf::Vector2f
					{
							static_cast<float>(tile_size_ * x),
							static_cast<float>(tile_size_ * y),
					};
					const auto scale = sf::Vector2f
					{
							static_cast<float>(tile_size_) / static_cast<float>(sprite_size.x),
							static_cast<float>(tile_size_) / static_cast<float>(sprite_size.y),
					};

					sprite.setPosition(position);
					sprite.setScale(scale);

					window.draw(sprite);
				}

				// 网格边框
				sf::RectangleShape tile_border{{static_cast<float>(tile_size_), static_cast<float>(tile_size_)}};
				tile_border.setPosition({static_cast<float>(x * tile_size_), static_cast<float>(y * tile_size_)});
				tile_border.setOutlineThickness(1);
				tile_border.setOutlineColor(sf::Color{100, 100, 100, 100});
				tile_border.setFillColor(sf::Color::Transparent);
				window.draw(tile_border);
			}
		}
	}
}
