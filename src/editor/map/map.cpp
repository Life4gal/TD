#include <map.hpp>

#include <algorithm>
#include <functional>
#include <ranges>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>

#include <SFML/Graphics.hpp>

#include <external/imgui-SFML.hpp>

#include <imgui.h>
#include <imgui_internal.h>

// ReSharper disable CppClangTidyBugproneSuspiciousStringviewDataUsage
// auto func(const char* string);
// func(string_view.data()); // <--- Result of a 'data()' call may not be null terminated, provide size information to the callee to prevent potential issues

namespace
{
	constexpr std::string_view window_name_tilemap{"TileMap"};
	constexpr std::string_view window_name_main{"Main"};
	constexpr std::string_view window_name_texture_atlas{"TextureAtlas"};
	constexpr std::string_view window_name_sprite_atlas{"SpriteAtlas"};
	constexpr std::string_view window_name_console{"Console"};

	constexpr std::string_view default_save_path{"save.png"};

	class ConsoleSink : public spdlog::sinks::base_sink<spdlog::details::null_mutex>
	{
	public:
		struct message_type
		{
			spdlog::level::level_enum level;
			std::string message;
		};

	private:
		inline static std::vector<message_type> messages_{};

		void sink_it_(const spdlog::details::log_msg& msg) final
		{
			spdlog::memory_buf_t formatted;
			formatter_->format(msg, formatted);

			messages_.emplace_back(msg.level, std::string{formatted.data(), formatted.size()});
		}

		void flush_() final
		{
			//
		}

	public:
		using base_sink::base_sink;

		[[nodiscard]] static auto messages() noexcept -> auto
		{
			return messages_ | std::views::all;
		}

		static auto clear() noexcept -> void
		{
			messages_.clear();
		}
	};

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
	Map::Layers::Layers() noexcept
		: elements{nullptr},
		  width{0},
		  height{0},
		  layers{nullptr},
		  // 至少一个图层
		  layers_count{1} {}

	auto Map::Layers::operator[](const size_type index) noexcept -> layer_type
	{
		assert(index < layers_count);

		return layers.get()[index];
	}

	auto Map::Layers::operator[](const size_type index) const noexcept -> const_layer_type
	{
		assert(index < layers_count);

		auto layer = layers.get()[index];

		return {layer.width(), layer.height(), layer.data()};
	}

	auto Map::Layers::build(const size_type init_width, const size_type init_height) noexcept -> void
	{
		width = init_width;
		height = init_height;

		// 一个图层元素数量
		const auto elements_per_layer = static_cast<std::size_t>(width) * height;
		// 所有图层元素数量
		const auto total_elements = elements_per_layer * layers_count;

		elements = std::make_unique<element_type[]>(total_elements);
		layers = std::make_unique<layer_type[]>(layers_count);

		// 指定所有图层视图
		for (size_type i = 0; i < layers_count; ++i)
		{
			auto* this_layer_element_begin = elements.get() + elements_per_layer * i;
			auto& this_layer = layers.get()[i];

			this_layer = utility::MatrixView{width, height, this_layer_element_begin};
		}
	}

	auto Map::Layers::rebuild(const size_type new_width, const size_type new_height) noexcept -> void
	{
		// 一个图层元素数量
		const auto elements_per_layer = static_cast<std::size_t>(new_width) * new_height;
		// 所有图层元素数量
		const auto total_elements = elements_per_layer * layers_count;

		auto [new_elements, new_layers] = [&]
		{
			if (new_width <= width and new_height <= height)
			{
				auto es = std::make_unique_for_overwrite<element_type[]>(total_elements);
				auto ls = std::make_unique_for_overwrite<layer_type[]>(layers_count);

				return std::make_pair(std::move(es), std::move(ls));
			}

			auto es = std::make_unique<element_type[]>(total_elements);
			auto ls = std::make_unique<layer_type[]>(layers_count);

			return std::make_pair(std::move(es), std::move(ls));
		}();

		// 指定所有图层视图
		for (size_type i = 0; i < layers_count; ++i)
		{
			auto* this_layer_element_begin = new_elements.get() + elements_per_layer * i;
			auto& this_layer = new_layers.get()[i];

			this_layer = utility::MatrixView{new_width, new_height, this_layer_element_begin};
		}

		// 复制原本的数据
		{
			std::ranges::subrange new_view{new_layers.get(), new_layers.get() + layers_count};
			std::ranges::subrange old_view{layers.get(), layers.get() + layers_count};

			for (auto [new_layer, old_layer]: std::views::zip(new_view, old_view))
			{
				new_layer.move_from(old_layer);
			}
		}

		// 替换数据
		elements = std::move(new_elements);
		layers = std::move(new_layers);

		width = new_width;
		height = new_height;
	}

	auto Map::Layers::append_layer(const size_type count) noexcept -> void
	{
		assert(count != 0);

		const auto previous_layers_count = layers_count;
		layers_count += count;

		// 一个图层元素数量
		const auto elements_per_layer = static_cast<std::size_t>(width) * height;
		// 所有图层元素数量
		const auto total_elements = elements_per_layer * layers_count;

		auto new_elements = std::make_unique<element_type[]>(total_elements);
		auto new_layers = std::make_unique<layer_type[]>(layers_count);

		// 指定所有图层视图
		for (size_type i = 0; i < layers_count; ++i)
		{
			auto* this_layer_element_begin = new_elements.get() + elements_per_layer * i;
			auto& this_layer = new_layers.get()[i];

			this_layer = utility::MatrixView{width, height, this_layer_element_begin};
		}

		// 复制原本的数据
		{
			std::ranges::subrange new_view{new_layers.get(), new_layers.get() + previous_layers_count};
			std::ranges::subrange old_view{layers.get(), layers.get() + previous_layers_count};

			for (auto [new_layer, old_layer]: std::views::zip(new_view, old_view))
			{
				new_layer.move_from(old_layer);
			}
		}

		// 替换数据
		elements = std::move(new_elements);
		layers = std::move(new_layers);
	}

	auto Map::Layers::save_layer(sf::RenderTexture& target, const Map::size_type tile_width, const Map::size_type tile_height, const layer_type& layer) noexcept -> void
	{
		for (layer_type::size_type y = 0; y < layer.height(); ++y)
		{
			for (layer_type::size_type x = 0; x < layer.width(); ++x)
			{
				const auto tile = layer[x, y];
				if (not tile.has_value())
				{
					continue;
				}

				auto& sprite = tile->get();
				const auto sprite_size = sprite.getTextureRect().size;

				const auto position = sf::Vector2f
				{
						static_cast<float>(tile_width * x),
						static_cast<float>(tile_height * y),
				};
				const auto scale = sf::Vector2f
				{
						static_cast<float>(tile_width) / static_cast<float>(sprite_size.x),
						static_cast<float>(tile_height) / static_cast<float>(sprite_size.y),
				};

				sprite.setPosition(position);
				sprite.setScale(scale);

				target.draw(sprite);
			}
		}
	}

	auto Map::Layers::save_all_layers(sf::RenderTexture& target, const Map::size_type tile_width, const Map::size_type tile_height) const noexcept -> void
	{
		const auto view = std::ranges::subrange{layers.get(), layers.get() + layers_count};

		std::ranges::for_each(
			view,
			[&](const layer_type& layer) noexcept -> void
			{
				save_layer(target, tile_width, tile_height, layer);
			}
		);
	}

	auto Map::save_image(const std::filesystem::path& path) const noexcept -> void
	{
		sf::RenderTexture render_texture{display_texture_.getSize()};
		render_texture.clear(sf::Color::Transparent);

		layers_.save_all_layers(render_texture, tile_width_, tile_height_);

		render_texture.display();

		if (const auto image = render_texture.getTexture().copyToImage();
			image.saveToFile(path))
		{
			SPDLOG_INFO("保存图片[{}]成功!", path.string());
		}
		else
		{
			// todo: 保存失败处理?
			SPDLOG_ERROR("保存图片[{}]失败!", path.string());
		}
	}

	auto Map::dock_setup() noexcept -> void
	{
		std::ignore = this;

		const auto* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

		ImGui::Begin(
			"DockSpace Window",
			nullptr,
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
		);

		ImGui::PopStyleVar(3);

		const auto dock_space_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dock_space_id, {0, 0}, ImGuiDockNodeFlags_None);

		ImGui::End();

		if (not dock_layout_initialized_)
		{
			dock_layout_initialized_ = true;

			// 删除之前的布局(如果有)
			ImGui::DockBuilderRemoveNode(dock_space_id);
			ImGui::DockBuilderAddNode(dock_space_id, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dock_space_id, viewport->WorkSize);

			auto main_id = dock_space_id;
			const auto left_id = ImGui::DockBuilderSplitNode(main_id, ImGuiDir_Left, .25f, nullptr, &main_id);
			auto right_id = ImGui::DockBuilderSplitNode(main_id, ImGuiDir_Right, .25f, nullptr, &main_id);
			const auto right_bottom_id = ImGui::DockBuilderSplitNode(right_id, ImGuiDir_Down, .5f, nullptr, &right_id);
			const auto bottom_id = ImGui::DockBuilderSplitNode(main_id, ImGuiDir_Down, .25f, nullptr, &main_id);

			ImGui::DockBuilderDockWindow(window_name_tilemap.data(), left_id);
			ImGui::DockBuilderDockWindow(window_name_main.data(), main_id);
			ImGui::DockBuilderDockWindow(window_name_texture_atlas.data(), right_id);
			ImGui::DockBuilderDockWindow(window_name_sprite_atlas.data(), right_bottom_id);
			ImGui::DockBuilderDockWindow(window_name_console.data(), bottom_id);

			ImGui::DockBuilderFinish(main_id);
		}
	}

	auto Map::dock_main_window() noexcept -> void
	{
		ImGui::Begin(window_name_main.data(), nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		{
			// 我们选择先渲染纹理,即使它可能已经被修改(将修改重建步骤延迟到下一帧)
			// 如此,我们可以很简单地确定我们当前是否想要修改它
			ImGui::Image(display_texture_);

			// 响应鼠标
			if (ImGui::IsItemHovered())
			{
				const auto image_position = ImGui::GetItemRectMin();
				// const auto image_size = ImGui::GetItemRectSize();

				// 左键设置瓦片
				const auto lmb_activated = ImGui::IsMouseClicked(ImGuiMouseButton_Left) or ImGui::IsMouseDragging(ImGuiMouseButton_Left);
				// 右键消除瓦片
				const auto rmb_activated = ImGui::IsMouseClicked(ImGuiMouseButton_Right) or ImGui::IsMouseDragging(ImGuiMouseButton_Right);

				if (lmb_activated or rmb_activated)
				{
					const auto mouse_absolute_position = ImGui::GetMousePos();
					const auto mouse_offset_on_image = sf::Vector2u
					{
							static_cast<unsigned>(mouse_absolute_position.x - image_position.x),
							static_cast<unsigned>(mouse_absolute_position.y - image_position.y),
					};

					const auto tile_x = mouse_offset_on_image.x / tile_width_;
					const auto tile_y = mouse_offset_on_image.y / tile_height_;

					auto layer = layers_[selected_layers_];
					auto& previous_selected = layer[tile_x, tile_y];

					if (lmb_activated)
					{
						if (selected_texture_atlas_iterator_ != texture_atlases_.end())
						{
							if (const auto& selected_texture_atlas = selected_texture_atlas_iterator_->second.second;
								selected_texture_atlas.has_selected())
							{
								const auto current_selected = selected_texture_atlas.get_selected();

								// 如果当前选择和之前不同则覆盖
								if (previous_selected != current_selected)
								{
									previous_selected = current_selected;

									display_texture_dirty_ = true;
								}
								else
								{
									if (previous_selected.has_value())
									{
										// todo: 让这些快捷键可配置?

										// 按住CTRL键填充全部
										if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) or ImGui::IsKeyDown(ImGuiKey_RightCtrl))
										{
											std::ranges::fill(layer, previous_selected);
										}

										// 按住左右箭头键填充一行
										if (ImGui::IsKeyDown(ImGuiKey_LeftArrow) or ImGui::IsKeyDown(ImGuiKey_RightArrow))
										{
											auto line = layer.line(tile_y);

											std::ranges::fill(line, previous_selected);
										}

										// 按住上下箭头键填充一列
										if (ImGui::IsKeyDown(ImGuiKey_UpArrow) or ImGui::IsKeyDown(ImGuiKey_DownArrow))
										{
											for (Layers::size_type y = 0; y < layer.height(); ++y)
											{
												layer[tile_x, y] = previous_selected;
											}
										}

										display_texture_dirty_ = true;
									}
								}
							}
						}
					}
					else if (rmb_activated)
					{
						previous_selected = std::nullopt;

						display_texture_dirty_ = true;
					}
				}
			}

			// 如果大小地图有变动则需要对应地设置纹理大小
			{
				const auto map_width = layers_.width * tile_width_;
				const auto map_height = layers_.height * tile_height_;

				if (const auto map_size = sf::Vector2u{map_width, map_height};
					display_texture_.getSize() != map_size)
				{
					const auto result = display_texture_.resize(map_size);
					assert(result);

					display_texture_.clear(sf::Color::Transparent);
					display_texture_dirty_ = true;
				}
			}

			// 绘制当前渲染层到纹理上
			if (display_texture_dirty_)
			{
				display_texture_.clear(sf::Color::Transparent);

				// // 网格边框(太细了)
				// sf::VertexArray lines{sf::PrimitiveType::Lines};
				// // 横向
				// for (Layers::size_type y = 0; y <= layers_.height; ++y)
				// {
				// 	const sf::Vector2f begin{0, static_cast<float>(tile_height_ * y)};
				// 	const sf::Vector2f end{static_cast<float>(tile_width_ * layers_.width), static_cast<float>(tile_height_ * y)};
				//
				// 	lines.append({.position = begin, .color = {100, 100, 100, 100}, .texCoords = {}});
				// 	lines.append({.position = end, .color = {100, 100, 100, 100}, .texCoords = {}});
				// }
				// // 纵向
				// for (Layers::size_type x = 0; x <= layers_.width; ++x)
				// {
				// 	const sf::Vector2f begin{static_cast<float>(tile_width_ * x), 0};
				// 	const sf::Vector2f end{static_cast<float>(tile_width_ * x), static_cast<float>(tile_height_ * layers_.height)};
				//
				// 	lines.append({.position = begin, .color = {100, 100, 100, 100}, .texCoords = {}});
				// 	lines.append({.position = end, .color = {100, 100, 100, 100}, .texCoords = {}});
				// }
				// display_texture_.draw(lines);
				// 网格边框(矩形模拟线段)
				sf::VertexArray lines{sf::PrimitiveType::Triangles};
				const auto push_quad = [&]<bool Horizontal>(const sf::Vector2f begin, const sf::Vector2f end) noexcept -> void
				{
					constexpr float line_width = 2.f;
					constexpr auto half_offset = line_width / 2;

					const auto offset = []
					{
						if constexpr (Horizontal)
						{
							return sf::Vector2f{0, half_offset};
						}
						else
						{
							return sf::Vector2f{half_offset, 0};
						}
					}();

					const auto p0 = begin - offset;
					const auto p1 = end - offset;
					const auto p2 = begin + offset;
					const auto p3 = end + offset;

					lines.append({.position = p0, .color = {100, 100, 100, 100}, .texCoords = {}});
					lines.append({.position = p1, .color = {100, 100, 100, 100}, .texCoords = {}});
					lines.append({.position = p3, .color = {100, 100, 100, 100}, .texCoords = {}});

					lines.append({.position = p0, .color = {100, 100, 100, 100}, .texCoords = {}});
					lines.append({.position = p2, .color = {100, 100, 100, 100}, .texCoords = {}});
					lines.append({.position = p3, .color = {100, 100, 100, 100}, .texCoords = {}});
				};
				// 横向
				for (Layers::size_type y = 0; y <= layers_.height; ++y)
				{
					const sf::Vector2f begin{0, static_cast<float>(tile_height_ * y)};
					const sf::Vector2f end{static_cast<float>(tile_width_ * layers_.width), static_cast<float>(tile_height_ * y)};

					push_quad.operator()<true>(begin, end);
				}
				// 纵向
				for (Layers::size_type x = 0; x <= layers_.width; ++x)
				{
					const sf::Vector2f begin{static_cast<float>(tile_width_ * x), 0};
					const sf::Vector2f end{static_cast<float>(tile_width_ * x), static_cast<float>(tile_height_ * layers_.height)};

					push_quad.operator()<false>(begin, end);
				}
				display_texture_.draw(lines);

				auto layer = layers_[selected_layers_];
				for (Layers::size_type y = 0; y < layers_.height; ++y)
				{
					for (Layers::size_type x = 0; x < layers_.width; ++x)
					{
						if (const auto tile = layer[x, y];
							tile.has_value())
						{
							auto& sprite = tile->get();

							const auto sprite_size = sprite.getTextureRect().size;

							const auto position = sf::Vector2f
							{
									static_cast<float>(tile_width_ * x),
									static_cast<float>(tile_height_ * y),
							};
							const auto scale = sf::Vector2f
							{
									static_cast<float>(tile_width_) / static_cast<float>(sprite_size.x),
									static_cast<float>(tile_height_) / static_cast<float>(sprite_size.y),
							};

							sprite.setPosition(position);
							sprite.setScale(scale);

							display_texture_.draw(sprite);
						}
					}
				}

				display_texture_.display();
				display_texture_dirty_ = false;
			}
		}
		ImGui::End();
	}

	auto Map::dock_tilemap_window() noexcept -> void
	{
		ImGui::Begin(window_name_tilemap.data());
		{
			// 设置参数
			{
				int layer_width = static_cast<int>(layers_.width);
				int layer_height = static_cast<int>(layers_.height);

				int tile_width = static_cast<int>(tile_width_);
				int tile_height = static_cast<int>(tile_height_);

				const auto layer_width_changed = ImGui::SliderInt("Set Layer Width", &layer_width, 1, 100);
				const auto layer_height_changed = ImGui::SliderInt("Set Layer Height", &layer_height, 1, 100);

				const auto tile_width_changed = ImGui::SliderInt("Set Tile Width", &tile_width, 1, 64);
				const auto tile_height_changed = ImGui::SliderInt("Set Tile Height", &tile_height, 1, 64);

				if (layer_width_changed or layer_height_changed)
				{
					layers_.rebuild(layer_width, layer_height);
				}

				if (tile_width_changed)
				{
					tile_width_ = tile_width;
				}
				if (tile_height_changed)
				{
					tile_height_ = tile_height;
				}
			}

			// 图层
			{
				if (ImGui::BeginCombo("Select Layer", std::format("{}", selected_layers_).c_str()))
				{
					for (Layers::size_type i = 0; i < layers_.layers_count; ++i)
					{
						if (const auto selected = i == selected_layers_;
							ImGui::Selectable(std::format("{}", i).c_str(), selected))
						{
							selected_layers_ = i;

							if (not selected)
							{
								// 添加脏标记,因为我们需要显示新的渲染层
								display_texture_dirty_ = true;
							}
						}
					}

					ImGui::EndCombo();
				}

				ImGui::SameLine();

				if (ImGui::Button("Add layer"))
				{
					layers_.append_layer(1);
				}
			}

			// 保存图片
			{
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
						// todo: 允许指定保存路径
						const auto path = std::filesystem::current_path() / full_filename;

						save_image(path);
					}

					// ImGui::SameLine();
					//
					// if (ImGui::Button("Preview"))
					// {
					// 	//
					// }
				}
			}
		}
		ImGui::End();
	}

	auto Map::dock_texture_atlas_window() noexcept -> void
	{
		ImGui::Begin(window_name_texture_atlas.data());
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
				if (ImGui::BeginCombo("Select TextureAtlas", selected_texture_atlas_iterator_->second.first.c_str()))
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
			}
		}
		ImGui::End();
	}

	auto Map::dock_sprite_atlas_window() noexcept -> void
	{
		if (selected_texture_atlas_iterator_ != texture_atlases_.end())
		{
			ImGui::Begin(window_name_sprite_atlas.data());
			{
				selected_texture_atlas_iterator_->second.second.show();
			}
			ImGui::End();
		}
	}

	auto Map::dock_console_window() const noexcept -> void
	{
		std::ignore = this;

		ImGui::Begin(window_name_console.data());
		{
			static bool show_errors = true;
			static bool show_warnings = true;
			static bool show_infos = true;
			static bool show_debugs = true;
			static bool auto_scroll = true;
			static ImGuiTextFilter text_filter{};

			if (ImGui::Button("Clear"))
			{
				ConsoleSink::clear();
			}

			ImGui::SameLine();
			ImGui::Checkbox("Error", &show_errors);

			ImGui::SameLine();
			ImGui::Checkbox("Warning", &show_warnings);

			ImGui::SameLine();
			ImGui::Checkbox("Info", &show_infos);

			ImGui::SameLine();
			ImGui::Checkbox("Debug", &show_debugs);

			ImGui::SameLine();
			ImGui::Checkbox("AutoScroll", &auto_scroll);

			ImGui::SameLine();
			text_filter.Draw("Filter", -100);

			ImGui::Separator();

			ImGui::BeginChild("Logs", {0, -ImGui::GetFrameHeightWithSpacing()});
			{
				const auto logs = ConsoleSink::messages();
				auto filtered_logs =
						logs |
						std::views::filter(
							[](const ConsoleSink::message_type& message) noexcept -> bool
							{
								const auto level = message.level;
								const auto context = std::string_view{message.message};

								if (
									(level == spdlog::level::err and not show_errors) or
									(level == spdlog::level::warn and not show_warnings) or
									(level == spdlog::level::info and not show_infos) or
									(level == spdlog::level::debug and not show_debugs)
								)
								{
									return false;
								}

								if (not text_filter.PassFilter(context.data(), context.data() + context.size()))
								{
									return false;
								}

								return true;
							}
						);
				const auto process_log = [](const ConsoleSink::message_type& message) noexcept -> void
				{
					const auto level = message.level;
					const auto context = std::string_view{message.message};

					ImU32 color;
					switch (level) // NOLINT(clang-diagnostic-switch-enum)
					{
						case spdlog::level::err:
						{
							color = IM_COL32(255, 0, 0, 255);
							break;
						}
						case spdlog::level::warn:
						{
							color = IM_COL32(200, 100, 0, 255);
							break;
						}
						case spdlog::level::info:
						{
							color = IM_COL32(0, 255, 0, 255);
							break;
						}
						case spdlog::level::debug:
						{
							color = IM_COL32(0, 0, 255, 255);
							break;
						}
						default:
						{
							color = IM_COL32(255, 255, 255, 255);
						}
					}

					ImGui::PushStyleColor(ImGuiCol_Text, color);
					ImGui::TextUnformatted(context.data(), context.data() + context.size());
					ImGui::PopStyleColor();
				};

				// std::ranges::for_each(filtered_logs, process_log);

				ImGuiListClipper list_clipper;
				list_clipper.Begin(static_cast<int>(std::ranges::distance(filtered_logs)));
				while (list_clipper.Step())
				{
					auto visible_logs =
							filtered_logs |
							std::views::drop(list_clipper.DisplayStart) |
							std::views::take(list_clipper.DisplayEnd - list_clipper.DisplayStart);

					std::ranges::for_each(visible_logs, process_log);
				}

				// 自动滚动到底部
				if (auto_scroll and ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				{
					ImGui::SetScrollHereY(1.0f);
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	Map::Map() noexcept
		: file_browser_{"MapEditor"},
		  save_image_filename_{.data = nullptr, .capacity = 0},
		  selected_texture_atlas_iterator_{texture_atlases_.end()},
		  selected_layers_{Layers::unselected},
		  tile_width_{0},
		  tile_height_{0},
		  display_texture_dirty_{false},
		  dock_layout_initialized_{false}
	{
		file_browser_.set_filter({".jpg", ".jpeg", ".png"});

		save_image_filename_ =
		{
				.data = std::make_unique_for_overwrite<char[]>(64),
				.capacity = 64,
		};
		save_image_filename_.data[0] = '\0';
	}

	auto Map::setup() noexcept -> void
	{
		// 配置日志器
		{
			const auto sink = std::make_shared<ConsoleSink>();
			const auto logger = std::make_shared<spdlog::logger>("console", sink);

			logger->set_level(spdlog::level::trace);
			logger->flush_on(spdlog::level::err);
			logger->set_pattern("[%H:%M:%S] %v");

			spdlog::set_default_logger(logger);
		}

		// 创建 20x20 的瓦片地图,每个瓦片大小为32x32像素
		tile_width_ = 32;
		tile_height_ = 32;
		layers_.build(20, 20);
		selected_layers_ = 0;
	}

	auto Map::update() noexcept -> void
	{
		// 文件选择器
		{
			file_browser_.show();

			if (file_browser_.has_selected())
			{
				auto selected = file_browser_.get_selected();
				file_browser_.clear_selected();

				// 载入纹理图集
				if (TextureAtlas atlas{selected}; atlas.loaded())
				{
					SPDLOG_INFO("载入纹理图集[{}]成功!", selected.string());

					auto filename = selected.filename().string();
					selected_texture_atlas_iterator_ = texture_atlases_.emplace(std::move(selected), std::make_pair(std::move(filename), std::move(atlas))).first;
				}
				else
				{
					SPDLOG_ERROR("载入纹理图集[{}]失败!", selected.string());

					// todo: 载入纹理失败后怎么做?
				}
			}
		}

		dock_setup();
		dock_main_window();
		dock_tilemap_window();
		dock_texture_atlas_window();
		dock_sprite_atlas_window();
		dock_console_window();
	}
}
