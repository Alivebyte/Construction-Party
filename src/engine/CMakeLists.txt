cmake_minimum_required(VERSION 3.19)
project(engine)

find_package(SDL2 CONFIG REQUIRED)

list(APPEND ENGINE_SOURCES
	# Core
	ilogger.h
	logger.cpp
	# Engine
	iengine.h
	engine.h
	engine.cpp
	engine_main.h 
	engine_main.cpp
	# Server (Entity Model)
	ientity.h
	entity.cpp
	# File system
	ifilesystem.h
	filesystem.h
	filesystem.cpp
	# Render
	irender.h
	gl_shared.h
	gl_shared.cpp
	gpu_buffer.h
	gpu_buffer.cpp
	render.h
	render.cpp
	renderdevice.h
	renderdevice.cpp
	texture2d.h
	texture2d.cpp
	texturesmanager.h
	texturesmanager.cpp
	shader.h
	shader.cpp
	shadersystem.h
	shadersystem.cpp
	modelsystem.h
	modelsystem.cpp
)
add_library(${PROJECT_NAME} SHARED ${ENGINE_SOURCES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
		glad
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

install( TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )