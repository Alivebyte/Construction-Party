cmake_minimum_required(VERSION 3.19)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

option(BUILD_EXE "Build game executable" ON)
option(BUILD_DEDICATED "Build server executable" OFF)
option(BUILD_ENGINE "Build engine library" ON)
option(BUILD_CLIENT "Build client library" ON)
option(BUILD_SERVER "Build server library" ON)
option(ENABLE_TRACY "Enable tracy profiling" OFF)

include(VcpkgIntegration)

if(BUILD_ENGINE)
    list(APPEND VCPKG_MANIFEST_FEATURES "engine")
endif()

if(BUILD_CLIENT)
    list(APPEND VCPKG_MANIFEST_FEATURES "client")
endif()

project(Contruction-Party)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<0:>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<0:>")

if(BUILD_EXE)
    add_subdirectory(src/game_exe)
endif()

if(BUILD_DEDICATED)
    add_subdirectory(src/dedicated)
endif()

if(BUILD_ENGINE)
	add_subdirectory(external/glad)
    add_subdirectory(src/engine)
endif()

if(BUILD_CLIENT)
    add_subdirectory(src/client)
endif()

if(BUILD_SERVER)
    add_subdirectory(src/server)
endif()