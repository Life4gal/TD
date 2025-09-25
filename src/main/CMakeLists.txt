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
	
	${CMAKE_CURRENT_SOURCE_DIR}/config/wave.hpp

	# =============================
	# COMPONENTS
	
	${CMAKE_CURRENT_SOURCE_DIR}/components/config.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/font.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/texture.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/sound.hpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/components/debug.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/components/tags.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/entity.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/components/sprite_frame.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/renderable.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/components/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/enemy.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/components/timed_life.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/components/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/observer.hpp

	${CMAKE_CURRENT_SOURCE_DIR}/components/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/graveyard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/components/hud.hpp
	
	# =============================
	# HELPER
	
	${CMAKE_CURRENT_SOURCE_DIR}/helper/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/map.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/helper/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/wave.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/helper/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/observer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/helper/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/resource.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/helper/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/player.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/helper/tower.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/tower.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/helper/enemy.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/helper/enemy.cpp

	# =============================
	# SYSTEMS
		
	${CMAKE_CURRENT_SOURCE_DIR}/systems/initialize.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/initialize.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/systems/debug.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/debug.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/systems/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/map.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/systems/wave.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/wave.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/systems/navigation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/navigation.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/systems/observer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/observer.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/systems/timed_life.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/timed_life.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/systems/weapon.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/weapon.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/systems/resource.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/resource.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/systems/player.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/player.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/systems/graveyard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/graveyard.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/systems/hud.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/hud.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/systems/sprite_frame.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/sprite_frame.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/systems/renderable.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/systems/renderable.cpp
	
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
