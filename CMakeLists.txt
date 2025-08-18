cmake_minimum_required(VERSION 3.25)

# https://learn.microsoft.com/zh-cn/vcpkg/users/buildsystems/cmake-integration
#set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_ROOT "C:/workspace/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# https://semver.org/lang/zh-CN/
set(TD_MAJOR_VERSION 0)
set(TD_MINOR_VERSION 1)
set(TD_PATCH_VERSION 0)
set(TD_VERSION "${TD_MAJOR_VERSION}.${TD_MINOR_VERSION}.${TD_PATCH_VERSION}")

project(
		TD
		VERSION ${TD_VERSION}
		DESCRIPTION "TD"
		HOMEPAGE_URL "https://github.com/Life4gal/TD"
		LANGUAGES CXX
)

# ===================================================================================================
# PLATFORM

# https://cmake.org/cmake/help/latest/variable/CMAKE_SYSTEM_NAME.html#system-names-known-to-cmake
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(TD_PLATFORM_WINDOWS ON)
	set(TD_PLATFORM_NAME TD_PLATFORM_WINDOWS)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(TD_PLATFORM_LINUX ON)
	set(TD_PLATFORM_NAME TD_PLATFORM_LINUX)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(TD_PLATFORM_DARWIN ON)
	set(TD_PLATFORM_NAME TD_PLATFORM_DARWIN)
else ()
	message(FATAL_ERROR "[TD] Unknown Platform: ${CMAKE_SYSTEM_NAME}")
endif (CMAKE_SYSTEM_NAME STREQUAL "Windows")

# ===================================================================================================
# COMPILER

# https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
# https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_FRONTEND_VARIANT.html
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(TD_COMPILER_MSVC ON)
	set(TD_COMPILER_NAME "MSVC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC" CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
		set(TD_COMPILER_CLANG_CL ON)
		set(TD_COMPILER_NAME "ClangCL")
	else ()
		set(TD_COMPILER_CLANG ON)
		set(TD_COMPILER_NAME "Clang")
	endif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC" CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(TD_COMPILER_GNU ON)
	set(TD_COMPILER_NAME "GNU")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	set(TD_COMPILER_CLANG_APPLE ON)
	set(TD_COMPILER_NAME "AppleClang")
else ()
	message(FATAL_ERROR "[TD] Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

# ===================================================================================================
# COMPILE FLAGS

if (TD_COMPILER_MSVC)
	set(TD_COMPILE_FLAGS "/D_CRT_SECURE_NO_WARNINGS;/DNOMINMAX;/utf-8;/W3;/WX;/Zc:preprocessor")
elseif (TD_COMPILER_CLANG_CL)
	set(TD_COMPILE_FLAGS "/D_CRT_SECURE_NO_WARNINGS;/DNOMINMAX;/utf-8;/W3;/WX")
elseif (TD_COMPILER_CLANG)
	set(TD_COMPILE_FLAGS "-Wall;-Wextra;-Wpedantic;-Werror")
elseif (TD_COMPILER_GNU)
	set(TD_COMPILE_FLAGS "-Wall;-Wextra;-Wpedantic;-Werror")
elseif (TD_COMPILER_CLANG_APPLE)
	set(TD_COMPILE_FLAGS "-Wall;-Wextra;-Wpedantic;-Werror")
endif (TD_COMPILER_MSVC)

# ===============================================
# GIT INFO

find_package(Git QUIET)
if (GIT_FOUND)
	# 获取commit hash
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE TD_GIT_COMMIT_HASH
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_QUIET
		RESULT_VARIABLE TD_GIT_REV_PARSE_RESULT
	)

	if (TD_GIT_REV_PARSE_RESULT EQUAL "0")
		# 检查是否有未提交的更改
		execute_process(
			COMMAND ${GIT_EXECUTABLE} diff --quiet --exit-code
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			RESULT_VARIABLE TD_GIT_DIRTY
			ERROR_QUIET
		)

		if (TD_GIT_DIRTY EQUAL "0")
			set(TD_GIT_DIRTY_FLAG "")
		else ()
			set(TD_GIT_DIRTY_FLAG "-dirty")
		endif (TD_GIT_DIRTY EQUAL "0")

		# 获取提交时间
		execute_process(
			COMMAND ${GIT_EXECUTABLE} log -1 --format=%cd --date=iso
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE TD_GIT_COMMIT_DATE
			OUTPUT_STRIP_TRAILING_WHITESPACE
			ERROR_QUIET
		)

		set(TD_GIT_COMMIT_INFO "${TD_GIT_COMMIT_HASH}${TD_GIT_DIRTY_FLAG} (${TD_GIT_COMMIT_DATE})")
	endif (TD_GIT_REV_PARSE_RESULT EQUAL "0")
endif (GIT_FOUND)

# ===============================================
# PRINT INFO

message(STATUS "[TD] CMAKE VERSION: ${CMAKE_VERSION}")
message(STATUS "[TD] Platform: ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "[TD] Project: ${PROJECT_NAME}-${TD_VERSION} (${CMAKE_BUILD_TYPE}) ${TD_GIT_COMMIT_INFO}")

# ===============================================
# DEPENDENCIES

# SFML
find_package(SFML 3 CONFIG REQUIRED COMPONENTS Graphics Audio)
find_package(OpenGL REQUIRED)
set(TD_SFML_LIBRARIES "SFML::Graphics;SFML::Audio;OpenGL::GL")

# IMGUI
find_package(imgui CONFIG REQUIRED)

# EnTT
find_package(EnTT CONFIG REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 复制 config/data/media的文件到输出目录
add_custom_target(
    copy_resources 
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/media ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media
)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/main)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/editor)
