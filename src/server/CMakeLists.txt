cmake_minimum_required(VERSION 3.19)
project(server)

find_package(glm CONFIG REQUIRED)
find_package(Jolt CONFIG REQUIRED)

set(ENGINE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/engine")
set(GAME_SHARED_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/game_shared")

list(APPEND SERVER_SOURCES
	# Core
	server_main.cpp
	
	# Game shared
	${GAME_SHARED_SOURCE_DIR}/baseentity.cpp
	${GAME_SHARED_SOURCE_DIR}/baseentity.h
	
	# Entity system
	server_entity.cpp
	server_entity.h

	# Physics
	physics_world.cpp
	physics_world.h
	
	# Entities
	player.cpp
	player.h
)

add_library(${PROJECT_NAME} SHARED ${SERVER_SOURCES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE ${ENGINE_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GAME_SHARED_SOURCE_DIR})

# TODO
target_link_libraries(${PROJECT_NAME} PRIVATE engine glm::glm Jolt::Jolt)

install( TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )