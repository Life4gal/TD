// This file is part of prometheus
// Copyright (C) 2022-2025 Life4gal <life4gal@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <external/imgui-file_browser.hpp>

#include <algorithm>
#include <cassert>
#include <format>
#include <fstream>
#include <optional>
#include <ranges>

#include <imgui.h>

namespace
{
	using ImGui::FileBrowser;

	template<typename Function>
		requires std::is_invocable_v<Function>
	class ScopeGuard final
	{
	public:
		ScopeGuard(const ScopeGuard&) noexcept = delete;
		ScopeGuard(ScopeGuard&&) noexcept = delete;
		auto operator=(const ScopeGuard&) noexcept -> ScopeGuard& = delete;
		auto operator=(ScopeGuard&&) noexcept -> ScopeGuard& = delete;

	private:
		Function function_;

	public:
		~ScopeGuard() noexcept
		{
			std::invoke(function_);
		}

		explicit ScopeGuard(Function function) noexcept
			: function_{function} {}
	};

	FileBrowser::size_type default_x{0};
	FileBrowser::size_type default_y{0};
	FileBrowser::size_type default_width{700};
	FileBrowser::size_type default_height{450};

	[[nodiscard]] auto make_title_label(const std::string_view title) noexcept -> std::string
	{
		return std::format("{}##FileBrowser", title);
	}

	auto expand_string_buffer(ImGuiInputTextCallbackData* callback_data) noexcept -> int
	{
		if (callback_data and callback_data->EventFlag & ImGuiInputTextFlags_CallbackResize)
		{
			if (auto* string = static_cast<FileBrowser::edit_string_buffer_type*>(callback_data->UserData);
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

	template<typename T>
		requires std::is_same_v<T, std::string> or std::is_same_v<T, std::string_view>
	constexpr auto do_set_filters(std::vector<std::string>& old_filters, const std::span<const T> new_filters) noexcept -> void
	{
		old_filters.clear();
		old_filters.reserve(new_filters.size() + 1);

		if (new_filters.size() > 1)
		{
			if (std::ranges::contains(new_filters, FileBrowser::wildcard_filter))
			{
				//
			}
			else
			{
				std::string combined_filter{};

				if constexpr (std::is_same_v<T, std::string>)
				{
					std::ranges::for_each(
						new_filters,
						[&](const std::string& filter) noexcept -> void
						{
							combined_filter.append(filter);
							combined_filter.push_back(',');
						}
					);
				}
				else if constexpr (std::is_same_v<T, std::string_view>)
				{
					std::ranges::for_each(
						new_filters,
						[&](const std::string_view filter) noexcept -> void
						{
							combined_filter.append(filter);
							combined_filter.push_back(',');
						}
					);
				}
				else
				{
					std::unreachable();
				}

				// pop ','
				combined_filter.pop_back();

				old_filters.push_back(std::move(combined_filter));
			}
		}

		if constexpr (std::is_same_v<T, std::string>)
		{
			std::ranges::copy(new_filters, std::back_inserter(old_filters));
		}
		else if constexpr (std::is_same_v<T, std::string_view>)
		{
			std::ranges::transform(
				new_filters,
				std::back_inserter(old_filters),
				[](const std::string_view filter) noexcept -> std::string
				{
					return std::string{filter};
				}
			);
		}
		else
		{
			std::unreachable();
		}
	}
}

// ReSharper disable once CppInconsistentNaming
namespace ImGui
{
	auto FileBrowser::has_state(const StateCategory state) const noexcept -> bool
	{
#if IMFB_DEBUG
		switch (state)
		{
			case StateCategory::NONE:
			{
				std::unreachable();
			}
			case StateCategory::POSITION_DIRTY:
			{
				return states_.position_dirty;
			}
			case StateCategory::OPENING:
			{
				return states_.window_opening;
			}
			case StateCategory::CLOSING:
			{
				return states_.window_closing;
			}
			case StateCategory::OPENED:
			{
				return states_.window_opened;
			}
			case StateCategory::SELECTED:
			{
				return states_.selected;
			}
			case StateCategory::FOCUSING_EDITOR_NEXT_FRAME:
			{
				return states_.focusing_editor_next_frame;
			}
			case StateCategory::SET_WORKING_DIRECTORY_NEXT_FRAME:
			{
				return states_.set_working_directory_next_frame;
			}
			case StateCategory::DELETE_SELECTED_NEXT_FRAME:
			{
				return states_.delete_selected_next_frame;
			}
			case StateCategory::SETTING_WORKING_DIRECTORY:
			{
				return states_.setting_working_directory;
			}
			case StateCategory::CREATING_FILE:
			{
				return states_.creating_file;
			}
			case StateCategory::CREATING_DIRECTORY:
			{
				return states_.creating_directory;
			}
			case StateCategory::CREATING:
			{
				return states_.creating_file or states_.creating_directory;
			}
			case StateCategory::RENAMING_FILE:
			{
				return states_.renaming_file;
			}
			case StateCategory::RENAMING_DIRECTORY:
			{
				return states_.renaming_directory;
			}
			case StateCategory::RENAMING:
			{
				return states_.renaming_file or states_.renaming_directory;
			}
			default:
			{
				std::unreachable();
			}
		}
#else
		assert(state != StateCategory::NONE);

		return std::to_underlying(states_) & std::to_underlying(state);
#endif
	}

	auto FileBrowser::append_state(const StateCategory state) noexcept -> void
	{
#if IMFB_DEBUG
		switch (state)
		{
			case StateCategory::NONE:
			{
				std::unreachable();
			}
			case StateCategory::POSITION_DIRTY:
			{
				states_.position_dirty = 1;
				break;
			}
			case StateCategory::OPENING:
			{
				states_.window_opening = 1;
				break;
			}
			case StateCategory::CLOSING:
			{
				states_.window_closing = 1;
				break;
			}
			case StateCategory::OPENED:
			{
				states_.window_opened = 1;
				break;
			}
			case StateCategory::SELECTED:
			{
				states_.selected = 1;
				break;
			}
			case StateCategory::FOCUSING_EDITOR_NEXT_FRAME:
			{
				states_.focusing_editor_next_frame = 1;
				break;
			}
			case StateCategory::SET_WORKING_DIRECTORY_NEXT_FRAME:
			{
				states_.set_working_directory_next_frame = 1;
				break;
			}
			case StateCategory::DELETE_SELECTED_NEXT_FRAME:
			{
				states_.delete_selected_next_frame = 1;
				break;
			}
			case StateCategory::SETTING_WORKING_DIRECTORY:
			{
				states_.setting_working_directory = 1;
				break;
			}
			case StateCategory::CREATING_FILE:
			{
				states_.creating_file = 1;
				break;
			}
			case StateCategory::CREATING_DIRECTORY:
			{
				states_.creating_directory = 1;
				break;
			}
			case StateCategory::CREATING:
			{
				std::unreachable();
			}
			case StateCategory::RENAMING_FILE:
			{
				states_.renaming_file = 1;
				break;
			}
			case StateCategory::RENAMING_DIRECTORY:
			{
				states_.renaming_directory = 1;
				break;
			}
			case StateCategory::RENAMING:
			{
				std::unreachable();
			}
			default:
			{
				std::unreachable();
			}
		}
#else
		assert(state != StateCategory::NONE);
		assert(state != StateCategory::CREATING);
		assert(state != StateCategory::RENAMING);

		states_ = static_cast<StateCategory>(std::to_underlying(states_) | std::to_underlying(state));
#endif
	}

	auto FileBrowser::clear_state(const StateCategory state) noexcept -> void
	{
#if IMFB_DEBUG
		switch (state)
		{
			case StateCategory::NONE:
			{
				std::unreachable();
			}
			case StateCategory::POSITION_DIRTY:
			{
				states_.position_dirty = 0;
				break;
			}
			case StateCategory::OPENING:
			{
				states_.window_opening = 0;
				break;
			}
			case StateCategory::CLOSING:
			{
				states_.window_closing = 0;
				break;
			}
			case StateCategory::OPENED:
			{
				states_.window_opened = 0;
				break;
			}
			case StateCategory::SELECTED:
			{
				states_.selected = 0;
				break;
			}
			case StateCategory::FOCUSING_EDITOR_NEXT_FRAME:
			{
				states_.focusing_editor_next_frame = 0;
				break;
			}
			case StateCategory::SET_WORKING_DIRECTORY_NEXT_FRAME:
			{
				states_.set_working_directory_next_frame = 0;
				break;
			}
			case StateCategory::DELETE_SELECTED_NEXT_FRAME:
			{
				states_.delete_selected_next_frame = 0;
				break;
			}
			case StateCategory::SETTING_WORKING_DIRECTORY:
			{
				states_.setting_working_directory = 0;
				break;
			}
			case StateCategory::CREATING_FILE:
			{
				states_.creating_file = 0;
				break;
			}
			case StateCategory::CREATING_DIRECTORY:
			{
				states_.creating_directory = 0;
				break;
			}
			case StateCategory::CREATING:
			{
				states_.creating_file = 0;
				states_.creating_directory = 0;
				break;
			}
			case StateCategory::RENAMING_FILE:
			{
				states_.renaming_file = 0;
				break;
			}
			case StateCategory::RENAMING_DIRECTORY:
			{
				states_.renaming_directory = 0;
				break;
			}
			case StateCategory::RENAMING:
			{
				states_.renaming_file = 0;
				states_.renaming_directory = 0;
				break;
			}
			default:
			{
				std::unreachable();
			}
		}
#else
		assert(state != StateCategory::NONE);

		states_ = static_cast<StateCategory>(std::to_underlying(states_) & ~std::to_underlying(state));
#endif
	}

	auto FileBrowser::is_state_editing() const noexcept -> bool
	{
		return
				has_state(StateCategory::SETTING_WORKING_DIRECTORY) or
				has_state(StateCategory::CREATING) or
				has_state(StateCategory::RENAMING);
	}

	auto FileBrowser::is_filter_matched(const std::filesystem::path& extension) const noexcept -> bool
	{
		if (filters_.empty())
		{
			return true;
		}

		if (selected_filter_ == 0)
		{
			if (has_combined_filter())
			{
				return std::ranges::any_of(
					// drop the combined filter
					filters_ | std::views::drop(1),
					[es = extension.string()](const std::string& filter) noexcept -> bool
					{
						return filter == es;
					}
				);
			}

			// wildcard
			return true;
		}

		const auto& filter = filters_[selected_filter_];
		return filter == extension;
	}

	auto FileBrowser::has_combined_filter() const noexcept -> bool
	{
		assert(not filters_.empty());

		return filters_.front() != wildcard_filter;
	}

	auto FileBrowser::update_file_descriptors() noexcept -> void
	{
		file_descriptors_.clear();

		// parent folder
		file_descriptors_.push_back({.name = "..", .extension = "", .display_name = std::string{parent_path_name}, .is_directory = true});

		std::error_code error_code{};
		auto directory_iterator = std::filesystem::directory_iterator{working_directory_, error_code};

		if (error_code)
		{
			tooltip_ = std::format(
				"Error occurred while iterate\n\t{}\n\t{}",
				working_directory_.string(),
				error_code.message()
			);
			return;
		}

		std::string tooltip{"Error occurred\n"};

		std::ranges::for_each(
			directory_iterator,
			[&](const std::filesystem::directory_entry& entry) noexcept -> void
			{
				file_descriptor descriptor{};

				if (entry.is_regular_file(error_code))
				{
					descriptor.is_directory = false;
				}
				else if (entry.is_directory(error_code))
				{
					descriptor.is_directory = true;
				}

				if (error_code)
				{
					descriptor.name = "???";
					descriptor.extension = ".?";

					descriptor.display_name = error_code.message();

					std::format_to(
						std::back_inserter(tooltip),
						"\t{}\n\t\t{}\n",
						entry.path().string(),
						error_code.message()
					);
				}
				else
				{
					const auto& entry_path = entry.path();

					descriptor.name = entry_path.filename();
					descriptor.extension = entry_path.extension();

					if (descriptor.is_directory)
					{
						descriptor.display_name = std::format("[DIR] {}", descriptor.name.string());
					}
					else
					{
						descriptor.display_name = descriptor.name.string();
					}
				}

				file_descriptors_.push_back(std::move(descriptor));
			}
		);

		if (error_code)
		{
			tooltip_ = std::move(tooltip);
		}

		if (file_descriptors_.size() > 2)
		{
			std::ranges::sort(
				// drop parent folder path
				file_descriptors_ | std::views::drop(1),
				[](const file_descriptor& lhs, const file_descriptor& rhs) noexcept -> bool
				{
					if (lhs.is_directory != rhs.is_directory)
					{
						return lhs.is_directory;
					}

					const auto to_lower_name = [](std::string& name) noexcept -> void
					{
						const auto to_lower = [](const char c) noexcept -> char
						{
							return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
						};

						std::ranges::transform(name, name.begin(), to_lower);
					};

					auto lhs_name = lhs.name.string();
					auto rhs_name = rhs.name.string();

					to_lower_name(lhs_name);
					to_lower_name(rhs_name);

					return lhs_name < rhs_name;
				}
			);
		}
	}

	auto FileBrowser::show_working_path() noexcept -> void
	{
		if (has_state(StateCategory::SETTING_WORKING_DIRECTORY))
		{
			if (has_state(StateCategory::FOCUSING_EDITOR_NEXT_FRAME))
			{
				ImGui::SetKeyboardFocusHere();
			}

			ImGui::PushItemWidth(-1);
			ImGui::InputText(
				"##working_directory_path",
				edit_working_directory_buffer_.data.get(),
				edit_working_directory_buffer_.capacity,
				ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_AutoSelectAll,
				expand_string_buffer,
				&edit_working_directory_buffer_
			);
			ImGui::PopItemWidth();

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				clear_state(StateCategory::SETTING_WORKING_DIRECTORY);

				std::error_code error_code{};

				do
				{
					const auto length = std::strlen(edit_working_directory_buffer_.data.get());
					edit_working_directory_buffer_.data[length] = '\0';

					const std::string_view view{edit_working_directory_buffer_.data.get(), edit_working_directory_buffer_.data.get() + static_cast<std::ptrdiff_t>(length)};

					// "C:\\workspace\\my_project\\" ==> "C:\\workspace\\my_project"
					if (view.ends_with('\\') or view.ends_with('/'))
					{
						edit_working_directory_buffer_.data[length - 1] = '\0';
					}

					std::filesystem::path path{view};
					if (is_directory(path, error_code))
					{
						working_directory_ = std::move(path);
						append_state(StateCategory::SET_WORKING_DIRECTORY_NEXT_FRAME);
						break;
					}

					if (error_code)
					{
						tooltip_ = std::format("Error occurred while processing\n\t{}\n{}", view, error_code.message());
						break;
					}

					auto parent_path = path.parent_path();
					if (is_directory(parent_path, error_code))
					{
						working_directory_ = std::move(parent_path);
						append_state(StateCategory::SET_WORKING_DIRECTORY_NEXT_FRAME);
						break;
					}

					if (error_code)
					{
						tooltip_ = std::format("Error occurred while processing\n\t{}\n{}", parent_path.string(), error_code.message());
						break;
					}

					tooltip_ = std::format("[{}] is not a valid directory", view);
				} while (false);
			}
		}
		else
		{
			std::filesystem::path combined_working_directory{};
			bool pressed = false;

			for (const auto [index, sub]: std::views::enumerate(working_directory_))
			{
				if (not pressed)
				{
					combined_working_directory /= sub;

#ifdef _WIN32
					if (index == 0)
					{
						// add '\'
						combined_working_directory /= "\\";
					}
#endif
				}

#ifdef _WIN32

				// skip '\'
				// e.g. 'C:' '\' 'workspace'
				if (index == 1)
				{
					assert(sub == "\\");
					continue;
				}
#endif

				ImGui::PushID(static_cast<int>(index));
				if (index > 0)
				{
					ImGui::SameLine();
				}
				const auto label = sub.string();
				pressed |= ImGui::SmallButton(label.c_str());
				ImGui::PopID();
			}

			if (pressed)
			{
				working_directory_ = combined_working_directory;
				append_state(StateCategory::SET_WORKING_DIRECTORY_NEXT_FRAME);
			}

			if (has_flag(FileBrowserFlags::ALLOW_SET_WORKING_DIRECTORY))
			{
				ImGui::SameLine();

				if (ImGui::SmallButton("#"))
				{
					tooltip_.clear();

					const auto working_directory_string = working_directory_.string();

					edit_working_directory_buffer_.capacity = working_directory_string.size() + 1;
					edit_working_directory_buffer_.data = std::make_unique_for_overwrite<char[]>(edit_working_directory_buffer_.capacity);
					edit_working_directory_buffer_.data[working_directory_string.size()] = '\0';

					std::ranges::copy(working_directory_string, edit_working_directory_buffer_.data.get());

					append_state(StateCategory::SETTING_WORKING_DIRECTORY);
					append_state(StateCategory::FOCUSING_EDITOR_NEXT_FRAME);
				}
				else if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Edit the current path");
				}
			}

			ImGui::SameLine();
			// Refresh
			if (ImGui::SmallButton("*"))
			{
				tooltip_.clear();

				update_file_descriptors();
			}
			else if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Refresh");
			}
		}
	}

	auto FileBrowser::show_tooltip() const noexcept -> void
	{
		if (not tooltip_.empty())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImColor{255, 0, 0}.operator ImVec4());
			ImGui::TextUnformatted(tooltip_.c_str(), tooltip_.c_str() + tooltip_.size());
			ImGui::PopStyleColor();
		}
	}

	auto FileBrowser::show_files_window_context() noexcept -> void
	{
		if (has_flag(FileBrowserFlags::ALLOW_CREATE))
		{
			if (ImGui::BeginPopupContextWindow(
				"file_context_menu",
				ImGuiPopupFlags_MouseButtonRight |
				ImGuiPopupFlags_NoOpenOverExistingPopup
			))
			{
				if (has_flag(FileBrowserFlags::ALLOW_CREATE_FILE) and ImGui::MenuItem("New file"))
				{
					tooltip_.clear();
					edit_create_file_or_directory_buffer_.data[0] = '\0';

					clear_selected();

					append_state(StateCategory::CREATING_FILE);
					append_state(StateCategory::FOCUSING_EDITOR_NEXT_FRAME);
				}

				if (has_flag(FileBrowserFlags::ALLOW_CREATE_DIRECTORY) and ImGui::MenuItem("New directory"))
				{
					tooltip_.clear();
					edit_create_file_or_directory_buffer_.data[0] = '\0';

					clear_selected();

					append_state(StateCategory::CREATING_DIRECTORY);
					append_state(StateCategory::FOCUSING_EDITOR_NEXT_FRAME);
				}

				ImGui::EndPopup();
			}
		}

		const auto select_directory = has_flag(FileBrowserFlags::SELECT_DIRECTORY);
		const auto hide_regular_files = select_directory and has_flag(FileBrowserFlags::HIDE_REGULAR_FILES);

		for (const auto& descriptor: file_descriptors_)
		{
			if (not descriptor.is_directory)
			{
				if (hide_regular_files)
				{
					continue;
				}

				if (not is_filter_matched(descriptor.extension))
				{
					continue;
				}
			}

			if (const auto selected = selected_filenames_.contains(descriptor.name);
				ImGui::Selectable(descriptor.display_name.c_str(), selected, ImGuiSelectableFlags_NoAutoClosePopups))
			{
				const auto selectable = descriptor.name != ".." and descriptor.is_directory == has_flag(FileBrowserFlags::SELECT_DIRECTORY);
				const auto multiple_select =
						has_flag(FileBrowserFlags::MULTIPLE_SELECTION) and
						ImGui::GetIO().KeyCtrl and
						ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

				if (selected)
				{
					if (multiple_select)
					{
						selected_filenames_.erase(descriptor.name);
					}
					else
					{
						selected_filenames_ = {descriptor.name};
					}
				}
				else if (selectable)
				{
					if (multiple_select)
					{
						selected_filenames_.insert(descriptor.name);
					}
					else
					{
						selected_filenames_ = {descriptor.name};
					}
				}
			}

			if (has_flag(FileBrowserFlags::ALLOW_RENAME) or has_flag(FileBrowserFlags::ALLOW_DELETE))
			{
				if (ImGui::BeginPopupContextItem(descriptor.name.string().c_str(), ImGuiPopupFlags_MouseButtonRight))
				{
					if (
						(has_flag(FileBrowserFlags::ALLOW_RENAME_FILE) == (not descriptor.is_directory)) or
						(has_flag(FileBrowserFlags::ALLOW_RENAME_DIRECTORY) == descriptor.is_directory)
					)
					{
						if (ImGui::MenuItem("Rename"))
						{
							selected_filenames_ = {descriptor.name};

							const auto filename_string = descriptor.name.string();

							edit_rename_file_or_directory_buffer_.capacity = filename_string.size() + 1;
							edit_rename_file_or_directory_buffer_.data = std::make_unique_for_overwrite<char[]>(edit_rename_file_or_directory_buffer_.capacity);
							edit_rename_file_or_directory_buffer_.data[filename_string.size()] = '\0';

							std::ranges::copy(filename_string, edit_rename_file_or_directory_buffer_.data.get());

							if (descriptor.is_directory)
							{
								append_state(StateCategory::RENAMING_DIRECTORY);
							}
							else
							{
								append_state(StateCategory::RENAMING_FILE);
							}
							append_state(StateCategory::FOCUSING_EDITOR_NEXT_FRAME);
						}
					}

					if (
						(has_flag(FileBrowserFlags::ALLOW_DELETE_FILE) == (not descriptor.is_directory)) or
						(has_flag(FileBrowserFlags::ALLOW_DELETE_DIRECTORY) == descriptor.is_directory)
					)
					{
						if (ImGui::MenuItem("Delete"))
						{
							selected_filenames_ = {descriptor.name};

							append_state(StateCategory::DELETE_SELECTED_NEXT_FRAME);
						}
					}

					ImGui::EndPopup();
				}
			}


			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) and ImGui::IsItemHovered())
			{
				if (descriptor.is_directory)
				{
					if (descriptor.name == "..")
					{
						working_directory_ = working_directory_.parent_path();
					}
					else
					{
						working_directory_ = working_directory_ / descriptor.name;
					}

					append_state(StateCategory::SET_WORKING_DIRECTORY_NEXT_FRAME);
				}
				else if (not has_flag(FileBrowserFlags::SELECT_DIRECTORY))
				{
					selected_filenames_ = {descriptor.name};

					append_state(StateCategory::SELECTED);
					ImGui::CloseCurrentPopup();
				}
			}
		}
	}

	auto FileBrowser::show_files_window_context_on_creating() noexcept -> void
	{
		const auto select_directory = has_flag(FileBrowserFlags::SELECT_DIRECTORY);
		const auto hide_regular_files = select_directory and has_flag(FileBrowserFlags::HIDE_REGULAR_FILES);

		for (const auto& descriptor: file_descriptors_)
		{
			if (not descriptor.is_directory)
			{
				if (hide_regular_files)
				{
					continue;
				}

				if (not is_filter_matched(descriptor.extension))
				{
					continue;
				}
			}

			ImGui::Selectable(descriptor.display_name.c_str(), false, ImGuiSelectableFlags_NoAutoClosePopups);
		}

		if (has_state(StateCategory::FOCUSING_EDITOR_NEXT_FRAME))
		{
			ImGui::SetKeyboardFocusHere();
		}

		ImGui::PushItemWidth(-1);
		ImGui::InputText(
			"##create_file_or_directory",
			edit_create_file_or_directory_buffer_.data.get(),
			edit_create_file_or_directory_buffer_.capacity,
			ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_AutoSelectAll,
			expand_string_buffer,
			&edit_create_file_or_directory_buffer_
		);
		ImGui::PopItemWidth();

		clear_state(StateCategory::FOCUSING_EDITOR_NEXT_FRAME);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			const auto file = has_state(StateCategory::CREATING_FILE);

			if (file)
			{
				clear_state(StateCategory::CREATING_FILE);
			}
			else
			{
				clear_state(StateCategory::CREATING_DIRECTORY);
			}

			const auto length = std::strlen(edit_create_file_or_directory_buffer_.data.get());
			edit_create_file_or_directory_buffer_.data[length] = '\0';

			if (length == 0)
			{
				if (file)
				{
					tooltip_ = "Empty file name, operation cancelled.";
				}
				else
				{
					tooltip_ = "Empty directory name, operation cancelled.";
				}
			}
			else
			{
				const std::string_view view{edit_create_file_or_directory_buffer_.data.get(), edit_create_file_or_directory_buffer_.data.get() + static_cast<std::ptrdiff_t>(length)};

				if (const auto full_path = working_directory_ / view;
					exists(full_path))
				{
					tooltip_ = std::format(
						"{} {} already exist, operation cancelled.",
						file ? "File" : "Directory",
						view
					);
				}
				else
				{
					if (file)
					{
						if (std::ofstream new_file{full_path};
							new_file.is_open())
						{
							new_file.close();
							update_file_descriptors();
						}
						else
						{
							const auto error_code = std::make_error_code(static_cast<std::errc>(errno));
							tooltip_ = std::format("Failed to create file\n\t{}\n{}", view, error_code.message());
						}
					}
					else
					{
						if (std::error_code error_code{};
							create_directory(full_path, error_code))
						{
							update_file_descriptors();
						}
						else
						{
							tooltip_ = std::format("Failed to create directory\n\t{}\n\t{}", full_path.string(), error_code.message());
						}
					}
				}
			}
		}
	}

	auto FileBrowser::show_files_window_context_on_renaming() noexcept -> void
	{
		const auto select_directory = has_flag(FileBrowserFlags::SELECT_DIRECTORY);
		const auto hide_regular_files = select_directory and has_flag(FileBrowserFlags::HIDE_REGULAR_FILES);

		for (const auto& descriptor: file_descriptors_)
		{
			if (not descriptor.is_directory)
			{
				if (hide_regular_files)
				{
					continue;
				}

				if (not is_filter_matched(descriptor.extension))
				{
					continue;
				}
			}

			if (descriptor.name != *selected_filenames_.begin())
			{
				ImGui::Selectable(descriptor.display_name.c_str(), false, ImGuiSelectableFlags_NoAutoClosePopups);
			}
			else
			{
				if (has_state(StateCategory::FOCUSING_EDITOR_NEXT_FRAME))
				{
					ImGui::SetKeyboardFocusHere();
				}

				ImGui::PushItemWidth(-1);
				ImGui::InputText(
					"##rename_file_or_directory",
					edit_rename_file_or_directory_buffer_.data.get(),
					edit_rename_file_or_directory_buffer_.capacity,
					ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_AutoSelectAll,
					expand_string_buffer,
					&edit_rename_file_or_directory_buffer_
				);
				ImGui::PopItemWidth();

				clear_state(StateCategory::FOCUSING_EDITOR_NEXT_FRAME);

				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					const auto file = has_state(StateCategory::RENAMING_FILE);

					if (file)
					{
						clear_state(StateCategory::RENAMING_FILE);
					}
					else
					{
						clear_state(StateCategory::RENAMING_DIRECTORY);
					}

					const auto length = std::strlen(edit_rename_file_or_directory_buffer_.data.get());
					edit_rename_file_or_directory_buffer_.data[length] = '\0';

					if (length == 0)
					{
						if (file)
						{
							tooltip_ = "Empty file name, operation cancelled.";
						}
						else
						{
							tooltip_ = "Empty directory name, operation cancelled.";
						}
					}
					else
					{
						const std::string_view view{edit_rename_file_or_directory_buffer_.data.get(), edit_rename_file_or_directory_buffer_.data.get() + static_cast<std::ptrdiff_t>(length)};

						const auto old_path = working_directory_ / descriptor.name;
						const auto new_path = working_directory_ / view;

						std::error_code error_code{};
						std::filesystem::rename(old_path, new_path, error_code);

						if (error_code)
						{
							tooltip_ = std::format(
								"Error occurred while renaming\n\t{} to {}\n{}",
								descriptor.name.string(),
								view,
								error_code.message()
							);
						}

						update_file_descriptors();
					}
				}
			}
		}
	}

	auto FileBrowser::show_files_window() noexcept -> void
	{
		const auto height = ImGui::GetFrameHeightWithSpacing();

		ImGui::BeginChild(
			"files",
			{0, -height},
			ImGuiChildFlags_Borders
		);
		ScopeGuard child_guard
		{
				[]
				{
					ImGui::EndChild();
				}
		};

		const auto creating_file_or_directory = has_state(StateCategory::CREATING) and (selected_filenames_.empty());
		const auto renaming_file_or_directory = has_state(StateCategory::RENAMING) and (selected_filenames_.size() == 1);

		assert(
			(creating_file_or_directory == false and renaming_file_or_directory == false) or
			(creating_file_or_directory != renaming_file_or_directory)
		);

		if (creating_file_or_directory)
		{
			show_files_window_context_on_creating();
		}
		else if (renaming_file_or_directory)
		{
			show_files_window_context_on_renaming();
		}
		else
		{
			show_files_window_context();
		}

		if (not is_state_editing())
		{
			if (const auto select_all =
						has_flag(FileBrowserFlags::MULTIPLE_SELECTION) and
						(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) or ImGui::IsKeyDown(ImGuiKey_RightCtrl)) and
						ImGui::IsKeyPressed(ImGuiKey_A);
				select_all)
			{
				selected_filenames_.clear();

				std::ranges::for_each(
					// drop parent folder path
					file_descriptors_ | std::views::drop(1),
					[&](const file_descriptor& descriptor) noexcept -> void
					{
						if (has_flag(FileBrowserFlags::SELECT_DIRECTORY))
						{
							if (descriptor.is_directory)
							{
								selected_filenames_.insert(descriptor.name);
							}
						}
						else
						{
							if (not descriptor.is_directory and is_filter_matched(descriptor.extension))
							{
								selected_filenames_.insert(descriptor.name);
							}
						}
					}
				);
			}
		}
	}

	auto FileBrowser::show_bottom_tools() noexcept -> void
	{
		// OK
		{
			const auto confirm_by_enter =
					has_flag(FileBrowserFlags::CONFIRM_ON_ENTER) and
					not is_state_editing() and
					// ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) and
					ImGui::IsWindowFocused(ImGuiFocusedFlags_NoPopupHierarchy) and
					ImGui::IsKeyPressed(ImGuiKey_Enter);

			if (has_flag(FileBrowserFlags::SELECT_DIRECTORY))
			{
				// select working directory anyway
				if (ImGui::Button("OK") or confirm_by_enter)
				{
					append_state(StateCategory::SELECTED);
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				ImGui::BeginDisabled(selected_filenames_.empty());
				const auto ok = ImGui::Button("OK");
				ImGui::EndDisabled();

				if ((ok or confirm_by_enter) and not selected_filenames_.empty())
				{
					append_state(StateCategory::SELECTED);
					ImGui::CloseCurrentPopup();
				}
			}
		}

		ImGui::SameLine();

		// Cancel
		{
			const auto close_by_escape =
					has_flag(FileBrowserFlags::CLOSE_ON_ESCAPE) and
					not is_state_editing() and
					// ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) and
					ImGui::IsWindowFocused(ImGuiFocusedFlags_NoPopupHierarchy) and
					ImGui::IsKeyPressed(ImGuiKey_Escape);

			if (ImGui::Button("Cancel") or has_state(StateCategory::CLOSING) or close_by_escape)
			{
				ImGui::CloseCurrentPopup();
			}
		}

		// Filters
		if (not filters_.empty())
		{
			ImGui::SameLine();
			ImGui::PushItemWidth(8 * ImGui::GetFontSize());
			if (ImGui::BeginCombo("##filters", filters_[selected_filter_].c_str()))
			{
				for (const auto [index, filter]: std::views::enumerate(filters_))
				{
					if (const auto selected = index == selected_filter_;
						ImGui::Selectable(filter.c_str(), selected) and not selected)
					{
						selected_filter_ = index;
					}
				}

				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
		}
	}

	FileBrowser::~FileBrowser() noexcept = default;

	FileBrowser::FileBrowser(
		const std::string_view title,
		const size_type x,
		const size_type y,
		const size_type width,
		const size_type height,
		const FileBrowserFlags flags,
		std::filesystem::path open_directory
	) noexcept
		: title_{title},
		  title_label_{make_title_label(title_)},
		  x_{x},
		  y_{y},
		  width_{width},
		  height_{height},
		  flags_{flags},
#if not IMFB_DEBUG
		  states_{StateCategory::NONE},
#endif
		  working_directory_{std::move(open_directory)},
		  edit_working_directory_buffer_{.data = nullptr, .capacity = 0},
		  edit_create_file_or_directory_buffer_{.data = nullptr, .capacity = 0},
		  edit_rename_file_or_directory_buffer_{.data = nullptr, .capacity = 0},
		  selected_filter_{0}
	{
#if IMFB_DEBUG
		std::memset(&states_, 0, sizeof(States::value_type));
#endif

		edit_create_file_or_directory_buffer_ =
		{
				.data = std::make_unique_for_overwrite<char[]>(64),
				.capacity = 64,
		};
		edit_create_file_or_directory_buffer_.data[0] = '\0';
	}

	FileBrowser::FileBrowser(
		const std::string_view title,
		const size_type width,
		const size_type height,
		const FileBrowserFlags flags,
		std::filesystem::path open_directory
	) noexcept
		: FileBrowser
		{
				title,
				default_x,
				default_y,
				width,
				height,
				flags,
				std::move(open_directory)
		} {}

	FileBrowser::FileBrowser(
		const std::string_view title,
		const FileBrowserFlags flags,
		std::filesystem::path open_directory
	) noexcept
		: FileBrowser
		{
				title,
				default_x,
				default_y,
				default_width,
				default_height,
				flags,
				std::move(open_directory)
		} {}

	FileBrowser::FileBrowser(
		const std::string_view title,
		std::filesystem::path open_directory
	) noexcept
		: FileBrowser
		{
				title,
				FileBrowserFlags::NONE,
				std::move(open_directory)
		} {}

	auto FileBrowser::get_title() const noexcept -> std::string_view
	{
		return title_;
	}

	auto FileBrowser::set_title(const std::string_view title) noexcept -> void
	{
		title_ = title;
		title_label_ = make_title_label(title_);
	}

	auto FileBrowser::get_position_x() const noexcept -> size_type
	{
		return x_;
	}

	auto FileBrowser::get_position_y() const noexcept -> size_type
	{
		return y_;
	}

	auto FileBrowser::set_position(const size_type x, const size_type y) noexcept -> void
	{
		set_position_x(x);
		set_position_y(y);
	}

	auto FileBrowser::set_position_x(const size_type x) noexcept -> void
	{
		x_ = x;
		append_state(StateCategory::POSITION_DIRTY);
	}

	auto FileBrowser::set_position_y(const size_type y) noexcept -> void
	{
		y_ = y;
		append_state(StateCategory::POSITION_DIRTY);
	}

	auto FileBrowser::get_size_width() const noexcept -> size_type
	{
		return width_;
	}

	auto FileBrowser::get_size_height() const noexcept -> size_type
	{
		return height_;
	}

	auto FileBrowser::set_size(const size_type width, const size_type height) noexcept -> void
	{
		set_size_width(width);
		set_size_height(height);
	}

	auto FileBrowser::set_size_width(const size_type width) noexcept -> void
	{
		assert(width > 0);

		width_ = width;
	}

	auto FileBrowser::set_size_height(const size_type height) noexcept -> void
	{
		assert(height > 0);

		height_ = height;
	}

	auto FileBrowser::has_flag(const FileBrowserFlags flag) const noexcept -> bool
	{
		return std::to_underlying(flags_) & std::to_underlying(flag);
	}

	auto FileBrowser::get_flags() const noexcept -> FileBrowserFlags
	{
		return flags_;
	}

	auto FileBrowser::append_flags(const FileBrowserFlags flags) noexcept -> void
	{
		flags_ = static_cast<FileBrowserFlags>(std::to_underlying(flags_) | std::to_underlying(flags));
	}

	auto FileBrowser::append_flags(const std::initializer_list<FileBrowserFlags> flags) noexcept -> void
	{
		std::ranges::for_each(
			flags,
			[this](const FileBrowserFlags flag) noexcept -> void
			{
				append_flags(flag);
			}
		);
	}

	auto FileBrowser::set_flags(const FileBrowserFlags flags) noexcept -> void
	{
		flags_ = flags;
	}

	auto FileBrowser::set_flags(const std::initializer_list<FileBrowserFlags> flags) noexcept -> void
	{
		set_flags(FileBrowserFlags::NONE);
		append_flags(flags);
	}

	auto FileBrowser::get_working_directory() const noexcept -> const std::filesystem::path&
	{
		return working_directory_;
	}

	auto FileBrowser::set_working_directory(const std::filesystem::path& directory) noexcept -> bool
	{
		std::error_code error_code{};
		auto new_working_directory = absolute(directory, error_code);

		if (error_code)
		{
			return false;
		}

		working_directory_ = std::move(new_working_directory);
		update_file_descriptors();
		return true;
	}

	auto FileBrowser::is_opened() const noexcept -> bool
	{
		return has_state(StateCategory::OPENED);
	}

	auto FileBrowser::open() noexcept -> void
	{
		update_file_descriptors();
		clear_selected();

		append_state(StateCategory::OPENING);
		clear_state(StateCategory::CLOSING);
	}

	auto FileBrowser::is_closed() const noexcept -> bool
	{
		return not is_opened();
	}

	auto FileBrowser::close() noexcept -> void
	{
		clear_selected();

		append_state(StateCategory::CLOSING);
		clear_state(StateCategory::OPENING);
	}

	auto FileBrowser::show() noexcept -> void
	{
		ImGui::PushID(this);
		ScopeGuard id_guard
		{
				[this]
				{
					clear_state(StateCategory::OPENING);
					clear_state(StateCategory::CLOSING);
					ImGui::PopID();
				}
		};

		if (has_state(StateCategory::OPENING))
		{
			ImGui::OpenPopup(title_label_.c_str());
		}
		clear_state(StateCategory::OPENED);

		if (has_state(StateCategory::OPENING) and has_flag(FileBrowserFlags::NO_MODAL))
		{
			if (has_state(StateCategory::POSITION_DIRTY))
			{
				ImGui::SetNextWindowPos({static_cast<float>(get_position_x()), static_cast<float>(get_position_y())});
			}
			ImGui::SetNextWindowSize({static_cast<float>(get_size_width()), static_cast<float>(get_size_height())});
		}
		else
		{
			if (has_state(StateCategory::POSITION_DIRTY))
			{
				ImGui::SetNextWindowPos({static_cast<float>(get_position_x()), static_cast<float>(get_position_y())}, ImGuiCond_FirstUseEver);
			}
			ImGui::SetNextWindowSize({static_cast<float>(get_size_width()), static_cast<float>(get_size_height())}, ImGuiCond_FirstUseEver);
		}

		if (has_flag(FileBrowserFlags::NO_MODAL))
		{
			if (not ImGui::BeginPopup(
				title_label_.c_str(),
				has_flag(FileBrowserFlags::NO_TITLEBAR) ? ImGuiWindowFlags_NoTitleBar : 0
			))
			{
				return;
			}
		}
		else
		{
			if (not ImGui::BeginPopupModal(
				title_label_.c_str(),
				nullptr,
				has_flag(FileBrowserFlags::NO_TITLEBAR) ? ImGuiWindowFlags_NoTitleBar : 0
			))
			{
				return;
			}
		}

		append_state(StateCategory::OPENED);
		ScopeGuard popup_guard
		{
				[]
				{
					ImGui::EndPopup();
				}
		};

		if (has_state(StateCategory::SET_WORKING_DIRECTORY_NEXT_FRAME))
		{
			set_working_directory(working_directory_);
			clear_state(StateCategory::SET_WORKING_DIRECTORY_NEXT_FRAME);

			clear_selected();
		}
		if (has_state(StateCategory::DELETE_SELECTED_NEXT_FRAME))
		{
			clear_state(StateCategory::DELETE_SELECTED_NEXT_FRAME);

			std::error_code error_code{};

			std::string tooltip{"Error occurred:\n"};
			for (const auto& filename: selected_filenames_)
			{
				const auto full_path = working_directory_ / filename;
				// remove(full_path, error_code);
				remove_all(full_path, error_code);

				if (error_code)
				{
					std::format_to(
						std::back_inserter(tooltip),
						"\t{}\n\t\t{}\n",
						filename.string(),
						error_code.message()
					);
				}
			}

			if (error_code)
			{
				tooltip_ = std::move(tooltip);
			}

			clear_selected();
			update_file_descriptors();
		}

		show_working_path();

		show_tooltip();

		show_files_window();

		show_bottom_tools();
	}

	auto FileBrowser::has_selected() const noexcept -> bool
	{
		return has_state(StateCategory::SELECTED);
	}

	auto FileBrowser::get_selected() const noexcept -> std::filesystem::path
	{
		if (selected_filenames_.empty())
		{
			return working_directory_;
		}

		return working_directory_ / *selected_filenames_.begin();
	}

	auto FileBrowser::get_all_selected() const noexcept -> std::vector<std::filesystem::path>
	{
		if (selected_filenames_.empty())
		{
			return {working_directory_};
		}

		std::vector<std::filesystem::path> results{};
		results.reserve(selected_filenames_.size());

		std::ranges::transform(
			selected_filenames_,
			std::back_inserter(results),
			[this](const std::filesystem::path& filename) noexcept -> std::filesystem::path
			{
				return working_directory_ / filename;
			}
		);

		return results;
	}

	auto FileBrowser::clear_selected() noexcept -> void
	{
		selected_filenames_.clear();

		clear_state(StateCategory::SELECTED);
	}

	auto FileBrowser::get_filter() const noexcept -> const std::vector<std::string>&
	{
		return filters_;
	}

	auto FileBrowser::set_filter(const std::span<const std::string_view> filters) noexcept -> void
	{
		do_set_filters(filters_, filters);
		selected_filter_ = 0;
	}

	auto FileBrowser::set_filter(const std::span<const std::string> filters) noexcept -> void
	{
		do_set_filters(filters_, filters);
		selected_filter_ = 0;
	}

	auto FileBrowser::set_filter(const std::vector<std::string>& filters) noexcept -> void
	{
		set_filter(std::span{filters.begin(), filters.end()});
	}

	auto FileBrowser::clear_filter() noexcept -> void
	{
		filters_.clear();
	}
}
