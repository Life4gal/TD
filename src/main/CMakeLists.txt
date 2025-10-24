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
	${CMAKE_CURRENT_SOURCE_DIR}/utility/functional.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/utility/time.hpp
	
	#===================
	# META

	${CMAKE_CURRENT_SOURCE_DIR}/meta/name.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/meta/enumeration.hpp

	#===================
	# MAP

	${CMAKE_CURRENT_SOURCE_DIR}/map/tile_map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/map/tile_map.cpp
	
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

	# =============================
	# LOADERS
	
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/path.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/path.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/loaders/config.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/config.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/font.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/font.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/texture.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/texture.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/sound.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/sound.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/music.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/loaders/music.cpp
	
	# =============================
	# CONFIG
	
	${CMAKE_CURRENT_SOURCE_DIR}/config/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/config/wave.hpp

	# =============================
	# COMPONENTS
	
	# =======
	# CORE

	${CMAKE_CURRENT_SOURCE_DIR}/components/core/tags.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/core/transform.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/core/sprite_frame.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/core/renderable.hpp
	
	# =======
	# COMBAT

	${CMAKE_CURRENT_SOURCE_DIR}/components/combat/unit.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/combat/limited_life.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/combat/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/combat/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/combat/health_bar.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/combat/weapon.hpp

	# =======
	# GAME

	${CMAKE_CURRENT_SOURCE_DIR}/components/game/asset.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/game/game.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/game/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/game/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/game/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/game/graveyard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/game/hud.hpp

	# =======
	# MAP

	${CMAKE_CURRENT_SOURCE_DIR}/components/map/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/map/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/map/observer.hpp
	
	# =============================
	# FACTORY

	${CMAKE_CURRENT_SOURCE_DIR}/factory/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/factory/enemy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/factory/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/factory/tower.cpp

	# =============================
	# HELPER
	
	${CMAKE_CURRENT_SOURCE_DIR}/helper/asset.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/asset.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/helper/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/wave.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/helper/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/observer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/helper/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/resource.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/helper/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/helper/combat_unit.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/combat_unit.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/helper/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/helper/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/enemy.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/helper/sprite_frame.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/sprite_frame.cpp
	
	# =============================
	# INITIALIZE

	${CMAKE_CURRENT_SOURCE_DIR}/initialize/map_data.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/map_data.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/wave_data.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/wave_data.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/initialize/asset.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/asset.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/initialize/event_connection.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/event_connection.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/game.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/game.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/map.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/navigation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/observer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/player.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/initialize/hud.cpp

	# =============================
	# UPDATE

	${CMAKE_CURRENT_SOURCE_DIR}/update/game.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/game.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/update/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/wave.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/navigation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/observer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/weapon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/limited_life.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/limited_life.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/sprite_frame.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/sprite_frame.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/update/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/player.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/graveyard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/graveyard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/resource.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/update/hud.cpp

	# =============================
	# RENDER	
	
	${CMAKE_CURRENT_SOURCE_DIR}/render/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/map.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/navigation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/renderable.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/renderable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/health_bar.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/health_bar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/player.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/render/hud.cpp

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

add_dependencies(${PROJECT_NAME} copy_resources)
