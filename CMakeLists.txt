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

# ===============================================
# EXECUTABLE

add_executable(
	${PROJECT_NAME}
	
	#===================
	# EXTERNAL

	${CMAKE_CURRENT_SOURCE_DIR}/src/external/imgui-SFML.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/external/imgui-SFML.cpp
	
	#===================
	# UTILITY

	${CMAKE_CURRENT_SOURCE_DIR}/src/utility/matrix.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/utility/hash.hpp

	#===================
	# MAP

	${CMAKE_CURRENT_SOURCE_DIR}/src/map/tile_map.hpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/map/path.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/map/path.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/map/flow_field.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/map/flow_field.cpp
	
	# =============================
	# SCENE
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/scene/scene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/scene/scene.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/scene/game.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/scene/game.cpp
	
	#===================
	# MANAGER
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/manager/font.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/manager/font.cpp
	
	#===================
	# COMPONENT
	
	# ============
	# 全局注册表(每个场景的注册表都会有一个全局注册表指针)
	# ============

	${CMAKE_CURRENT_SOURCE_DIR}/src/component/registry.hpp
	
	# ============
	# 主菜单
	# ============
	
	# ============
	# 游戏主场景
	# ============
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/tags.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/entity.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/graveyard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/hud.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/weapon.hpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/component/game/render.hpp
	
	#===================
	# SYSTEM
	
	# ============
	# 主菜单
	# ============
	
	# ============
	# 游戏主场景
	# ============
	
	# =============
	# helper

	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/map.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/observer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/resource.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/enemy.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/helper/weapon.cpp

	# =============
	# initialize

	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/map.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/navigation.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/observer.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/resource.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/hud.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/enemy.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/initialize/weapon.cpp

	# =============
	# update
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/map.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/navigation.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/observer.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/graveyard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/graveyard.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/resource.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/hud.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/enemy.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/update/weapon.cpp

	# =============
	# render
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/map.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/navigation.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/hud.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/enemy.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/system/game/render/weapon.cpp
	
	#===================
	# MAIN

	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC

	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(
	${PROJECT_NAME}
	PUBLIC

	${TD_COMPILE_FLAGS}
)

target_compile_definitions(
	${PROJECT_NAME}
	PUBLIC

	TD_MAJOR_VERSION=${TD_MAJOR_VERSION}
	TD_MINOR_VERSION=${TD_MINOR_VERSION}
	TD_PATCH_VERSION=${TD_PATCH_VERSION}
	TD_VERSION="${TD_VERSION}"

	TD_PROJECT_NAME="${PROJECT_NAME}"
	TD_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
	TD_GIT_COMMIT_INFO="${TD_GIT_COMMIT_INFO}"

	${TD_PLATFORM_NAME}

	# https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html

	# MSVC
	$<$<CXX_COMPILER_ID:MSVC>:TD_COMPILER_MSVC>
	# GNU(g++)
	$<$<CXX_COMPILER_ID:GNU>:TD_COMPILER_GNU>
	# ClangCL
	$<$<AND:$<CXX_COMPILER_ID:Clang>,$<OR:$<STREQUAL:CMAKE_CXX_COMPILER_FRONTEND_VARIANT,MSVC>,$<STREQUAL:CMAKE_CXX_SIMULATE_ID,MSVC>>>:TD_COMPILER_CLANG_CL>
	# Clang
	$<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<OR:$<STREQUAL:CMAKE_CXX_COMPILER_FRONTEND_VARIANT,MSVC>,$<STREQUAL:CMAKE_CXX_SIMULATE_ID,MSVC>>>>:TD_COMPILER_CLANG>
	# AppleClang
	$<$<CXX_COMPILER_ID:AppleClang>:TD_COMPILER_CLANG_APPLE>
)

target_compile_features(
	${PROJECT_NAME}
	PRIVATE

	cxx_std_23
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE

	${TD_SFML_LIBRARIES}
	imgui::imgui
	EnTT::EnTT
)
