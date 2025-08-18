// This file is part of prometheus
// Copyright (C) 2022-2025 Life4gal <life4gal@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#pragma once

#if not defined(IMFB_DEBUG)

#if defined(DEBUG) or defined(_DEBUG)
#define IMFB_DEBUG 1
#endif

#endif

#include <vector>
#include <filesystem>
#include <span>
#include <unordered_set>

// ReSharper disable once CppInconsistentNaming
namespace ImGui
{
	enum class FileBrowserFlags : std::uint32_t
	{
		NONE = 0,

		// ============================
		// WINDOW
		// ============================

		// 0~3

		NO_TITLEBAR = 1 << 0,
		NO_MODAL = 1 << 1,

		// ============================
		// SELECTION
		// ============================

		// 4~7

		SELECT_DIRECTORY = 1 << 4,
		HIDE_REGULAR_FILES = 1 << 5,
		MULTIPLE_SELECTION = 1 << 6,

		// ============================
		// INTERACTIVE
		// ============================

		// 8~15

		CLOSE_ON_ESCAPE = 1 << 8,
		CONFIRM_ON_ENTER = 1 << 9,

		ALLOW_SET_WORKING_DIRECTORY = 1 << 10,

		ALLOW_CREATE_FILE = 1 << 11,
		ALLOW_CREATE_DIRECTORY = 1 << 12,
		ALLOW_CREATE = ALLOW_CREATE_FILE | ALLOW_CREATE_DIRECTORY,

		ALLOW_RENAME_FILE = 1 << 13,
		ALLOW_RENAME_DIRECTORY = 1 << 14,
		ALLOW_RENAME = ALLOW_RENAME_FILE | ALLOW_RENAME_DIRECTORY,

		ALLOW_DELETE_FILE = 1 << 15,
		ALLOW_DELETE_DIRECTORY = 1 << 16,
		ALLOW_DELETE = ALLOW_DELETE_FILE | ALLOW_DELETE_DIRECTORY,
	};

	class FileBrowser final
	{
	public:
		using size_type = int;

		struct edit_string_buffer_type
		{
			std::unique_ptr<char[]> data;
			std::size_t capacity;
		};

		constexpr static std::string_view parent_path_name{"(last level)"};

		constexpr static std::string_view wildcard_filter{".*"};

	private:
		enum class StateCategory : std::uint32_t
		{
			NONE = 0,

			// ========================
			// STATUS
			// ========================

			// 0~3

			POSITION_DIRTY = 1 << 0,

			// ========================
			// WINDOW
			// ========================

			// 4~7

			OPENING = 1 << 4,
			CLOSING = 1 << 5,
			OPENED = 1 << 6,

			// ========================
			// SELECTION
			// ========================

			// 8~11

			SELECTED = 1 << 8,

			// ========================
			// INTERACTIVE
			// ========================

			// 12~15

			// focus the editor(InputText) next frame
			FOCUSING_EDITOR_NEXT_FRAME = 1 << 12,
			// double click selectable ==> change directory
			SET_WORKING_DIRECTORY_NEXT_FRAME = 1 << 13,
			// right click selectable ==> delete file/directory
			DELETE_SELECTED_NEXT_FRAME = 1 << 14,

			// 16~23

			// editing working directory editor(InputText)
			SETTING_WORKING_DIRECTORY = 1 << 16,

			// editing new file or directory editor(InputText)
			CREATING_FILE = 1 << 17,
			CREATING_DIRECTORY = 1 << 18,
			CREATING = CREATING_FILE | CREATING_DIRECTORY,

			// editing rename file or directory editor(InputText)
			RENAMING_FILE = 1 << 19,
			RENAMING_DIRECTORY = 1 << 20,
			RENAMING = RENAMING_FILE | RENAMING_DIRECTORY,
		};

#if IMFB_DEBUG
		// better for debugging
		class States
		{
		public:
			using value_type = std::uint32_t;

			// ========================
			// STATUS
			// ========================

			// 0~3

			value_type position_dirty : 1;
			value_type reserved_status_1 : 1;
			value_type reserved_status_2 : 1;
			value_type reserved_status_3 : 1;

			// ========================
			// WINDOW
			// ========================

			// 4~7

			value_type window_opening : 1;
			value_type window_closing : 1;
			value_type window_opened : 1;
			value_type reserved_window_7 : 1;

			// ========================
			// SELECTION
			// ========================

			// 8~11

			value_type selected : 1;
			value_type reserved_selection_9 : 1;
			value_type reserved_selection_10 : 1;
			value_type reserved_selection_11 : 1;

			// ========================
			// INTERACTIVE
			// ========================

			// 12~15

			// focus the editor(InputText) next frame
			value_type focusing_editor_next_frame : 1;
			// double click selectable ==> change directory
			value_type set_working_directory_next_frame : 1;
			// right click selectable ==> delete file/directory
			value_type delete_selected_next_frame : 1;
			value_type reserved_interactive_15 : 1;

			// 16~23

			// editing working directory editor(InputText)
			value_type setting_working_directory : 1;

			// editing new file or directory editor(InputText)
			value_type creating_file : 1;
			value_type creating_directory : 1;

			// editing rename file or directory editor(InputText)
			value_type renaming_file : 1;
			value_type renaming_directory : 1;

			value_type reserved : 11;
		};
#endif

		struct file_descriptor
		{
			std::filesystem::path name;
			std::filesystem::path extension;

			std::string display_name;
			bool is_directory;
		};

		std::string title_;
		// ImGui::FileBrowser file_browser{"FileBrowser"};
		// 
		// ImGui::Begin("FileBrowser");
		//
		// file_browser.show(); <- Error: ImGui::EndPopup(); ==> IM_ASSERT(window->Flags & ImGuiWindowFlags_Popup);
		//
		// ImGui::End();
		std::string title_label_;

		size_type x_;
		size_type y_;
		size_type width_;
		size_type height_;

		FileBrowserFlags flags_;
		// avoid padding
#if IMFB_DEBUG
		States states_;
#else
		StateCategory states_;
#endif

		// ========================
		// path
		// ========================

		std::filesystem::path working_directory_;

		// ========================
		// interactive
		// ========================

		edit_string_buffer_type edit_working_directory_buffer_;
		edit_string_buffer_type edit_create_file_or_directory_buffer_;
		edit_string_buffer_type edit_rename_file_or_directory_buffer_;

		// ========================
		// selection
		// ========================

		std::unordered_set<std::filesystem::path> selected_filenames_;

		// ========================
		// filter
		// ========================

		// [0]: wildcard filter (*)
		// [0]: combined filter
		std::vector<std::string> filters_;
		std::vector<std::string>::difference_type selected_filter_;

		// ========================
		// file descriptor
		// ========================

		std::vector<file_descriptor> file_descriptors_;

		// ========================
		// tooltip
		// ========================

		std::string tooltip_;

		// ========================
		// state
		// ========================

		[[nodiscard]] auto has_state(StateCategory state) const noexcept -> bool;

		auto append_state(StateCategory state) noexcept -> void;

		auto clear_state(StateCategory state) noexcept -> void;

		// editing something(InputText)
		[[nodiscard]] auto is_state_editing() const noexcept -> bool;

		// ========================
		// filter
		// ========================

		[[nodiscard]] auto is_filter_matched(const std::filesystem::path& extension) const noexcept -> bool;

		[[nodiscard]] auto has_combined_filter() const noexcept -> bool;

		// ========================
		// file descriptor
		// ========================

		auto update_file_descriptors() noexcept -> void;

		// ========================
		// show
		// ========================

		auto show_working_path() noexcept -> void;

		auto show_tooltip() const noexcept -> void;

		auto show_files_window_context() noexcept -> void;

		auto show_files_window_context_on_creating() noexcept -> void;

		auto show_files_window_context_on_renaming() noexcept -> void;

		auto show_files_window() noexcept -> void;

		auto show_bottom_tools() noexcept -> void;

	public:
		FileBrowser(const FileBrowser&) noexcept = delete;
		FileBrowser(FileBrowser&&) noexcept = default;
		auto operator=(const FileBrowser&) noexcept -> FileBrowser& = delete;
		auto operator=(FileBrowser&&) noexcept -> FileBrowser& = default;

		~FileBrowser() noexcept;

		explicit FileBrowser(
			std::string_view title,
			size_type x,
			size_type y,
			size_type width,
			size_type height,
			FileBrowserFlags flags,
			std::filesystem::path open_directory = std::filesystem::current_path()
		) noexcept;

		explicit FileBrowser(
			std::string_view title,
			size_type width,
			size_type height,
			FileBrowserFlags flags,
			std::filesystem::path open_directory = std::filesystem::current_path()
		) noexcept;

		explicit FileBrowser(
			std::string_view title,
			FileBrowserFlags flags,
			std::filesystem::path open_directory = std::filesystem::current_path()
		) noexcept;

		explicit FileBrowser(
			std::string_view title,
			std::filesystem::path open_directory = std::filesystem::current_path()
		) noexcept;

		// ========================
		// title
		// ========================

		[[nodiscard]] auto get_title() const noexcept -> std::string_view;

		auto set_title(std::string_view title) noexcept -> void;

		// ========================
		// position
		// ========================

		[[nodiscard]] auto get_position_x() const noexcept -> size_type;

		[[nodiscard]] auto get_position_y() const noexcept -> size_type;

		auto set_position(size_type x, size_type y) noexcept -> void;

		auto set_position_x(size_type x) noexcept -> void;

		auto set_position_y(size_type y) noexcept -> void;

		// ========================
		// size
		// ========================

		[[nodiscard]] auto get_size_width() const noexcept -> size_type;

		[[nodiscard]] auto get_size_height() const noexcept -> size_type;

		auto set_size(size_type width, size_type height) noexcept -> void;

		auto set_size_width(size_type width) noexcept -> void;

		auto set_size_height(size_type height) noexcept -> void;

		// ========================
		// flags
		// ========================

		[[nodiscard]] auto has_flag(FileBrowserFlags flag) const noexcept -> bool;

		[[nodiscard]] auto get_flags() const noexcept -> FileBrowserFlags;

		auto append_flags(FileBrowserFlags flags) noexcept -> void;

		auto append_flags(std::initializer_list<FileBrowserFlags> flags) noexcept -> void;

		template<typename... T>
			requires(std::is_same_v<T, FileBrowserFlags> and ...)
		auto append_flags(T... flags) noexcept -> void
		{
			(append_flags(flags), ...);
		}

		auto set_flags(FileBrowserFlags flags) noexcept -> void;

		auto set_flags(std::initializer_list<FileBrowserFlags> flags) noexcept -> void;

		template<typename... T>
			requires(std::is_same_v<T, FileBrowserFlags> and ...)
		auto set_flags(T... flags) noexcept -> void
		{
			set_flags(FileBrowserFlags::NONE);
			append_flags(flags...);
		}

		// ========================
		// path
		// ========================

		[[nodiscard]] auto get_working_directory() const noexcept -> const std::filesystem::path&;

		auto set_working_directory(const std::filesystem::path& directory = std::filesystem::current_path()) noexcept -> bool;

		// ========================
		// window
		// ========================

		[[nodiscard]] auto is_opened() const noexcept -> bool;

		auto open() noexcept -> void;

		[[nodiscard]] auto is_closed() const noexcept -> bool;

		auto close() noexcept -> void;

		auto show() noexcept -> void;

		// ========================
		// selection
		// ========================

		[[nodiscard]] auto has_selected() const noexcept -> bool;

		[[nodiscard]] auto get_selected() const noexcept -> std::filesystem::path;

		[[nodiscard]] auto get_all_selected() const noexcept -> std::vector<std::filesystem::path>;

		auto clear_selected() noexcept -> void;

		// ========================
		// filter
		// ========================

		[[nodiscard]] auto get_filter() const noexcept -> const std::vector<std::string>&;

		auto set_filter(std::span<const std::string_view> filters) noexcept -> void;

		auto set_filter(std::span<const std::string> filters) noexcept -> void;

		auto set_filter(const std::vector<std::string>& filters) noexcept -> void;

		auto clear_filter() noexcept -> void;
	};
}
