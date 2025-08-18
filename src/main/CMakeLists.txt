project(main)

add_executable(
	${PROJECT_NAME}
	
	#===================
	# EXTERNAL

	${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-SFML.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-SFML.cpp
	
	#===================
	# UTILITY

	${CMAKE_CURRENT_SOURCE_DIR}/utility/matrix.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/utility/hash.hpp

	#===================
	# MAP

	${CMAKE_CURRENT_SOURCE_DIR}/map/tile_map.hpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/map/path.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/map/path.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/map/flow_field.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/map/flow_field.cpp
	
	# ==========================
	# SCENE
	
	${CMAKE_CURRENT_SOURCE_DIR}/scene/scene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/scene/scene.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/scene/game.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/scene/game.cpp
	
	#===================
	# MANAGER
	
	${CMAKE_CURRENT_SOURCE_DIR}/manager/font.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/manager/font.cpp
	
	#===================
	# COMPONENT
	
	# ============
	# 全局注册表(每个场景的注册表都会有一个全局注册表指针)
	# ============

	${CMAKE_CURRENT_SOURCE_DIR}/component/registry.hpp
	
	# ============
	# 主菜单
	# ============
	
	# ============
	# 游戏主场景
	# ============
	
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/tags.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/entity.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/component/game/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/graveyard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/hud.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/component/game/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/weapon.hpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/component/game/render.hpp
	
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

	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/map.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/wave.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/observer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/resource.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/enemy.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/helper/weapon.cpp

	# =============
	# initialize

	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/map.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/wave.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/navigation.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/observer.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/resource.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/hud.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/enemy.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/initialize/weapon.cpp

	# =============
	# update
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/map.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/wave.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/navigation.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/observer.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/graveyard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/graveyard.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/resource.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/hud.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/enemy.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/update/weapon.cpp

	# =============
	# render
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/map.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/navigation.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/hud.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/enemy.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system/game/render/weapon.cpp
	
	#===================
	# MAIN

	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC

	${CMAKE_CURRENT_SOURCE_DIR}/
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
