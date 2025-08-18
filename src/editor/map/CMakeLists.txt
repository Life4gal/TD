project(MapEditor)

add_executable(
	${PROJECT_NAME}

	# =============================
	# EXTERNAL

	${CMAKE_SOURCE_DIR}/src/main/external/imgui-SFML.hpp
	${CMAKE_SOURCE_DIR}/src/main/external/imgui-SFML.cpp
	${CMAKE_SOURCE_DIR}/src/main/external/imgui-file_browser.hpp
	${CMAKE_SOURCE_DIR}/src/main/external/imgui-file_browser.cpp
	
	# =============================
	# UTILITY

	${CMAKE_SOURCE_DIR}/src/main/utility/matrix.hpp

	# =============================
	# MAIN
	
	${CMAKE_CURRENT_SOURCE_DIR}/sprite_region_selector.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sprite_region_selector.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/texture_atlas.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/texture_atlas.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/map.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/map.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(
	${PROJECT_NAME} 
	PUBLIC 

	${CMAKE_SOURCE_DIR}/src/main
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(
		${PROJECT_NAME}
		PUBLIC

		${TD_COMPILE_FLAGS}
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
)

set_target_properties(
	${PROJECT_NAME} 
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
