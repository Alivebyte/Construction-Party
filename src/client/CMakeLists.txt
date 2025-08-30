cmake_minimum_required(VERSION 3.19)
project(client)

find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

set(ENGINE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/engine")
set(GAME_SHARED_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/game_shared")

list(APPEND CLIENT_SOURCES
	# Core
	client_game.cpp
	
	# ImGui
	imgui_impl_engine.cpp
	imgui_impl_engine.h
	imgui_impl_sdl2.cpp
	imgui_impl_sdl2.h

	# Game shared
	${GAME_SHARED_SOURCE_DIR}/baseentity.cpp
	${GAME_SHARED_SOURCE_DIR}/baseentity.h

)

add_library(${PROJECT_NAME} SHARED ${CLIENT_SOURCES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE ${ENGINE_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GAME_SHARED_SOURCE_DIR})

# TODO
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui engine glm::glm $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
