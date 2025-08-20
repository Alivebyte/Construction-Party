cmake_minimum_required(VERSION 3.19)
project(engine)

list(APPEND ENGINE_SOURCES
	iengine.h
	engine.h
	engine.cpp
	engine_main.h 
	engine_main.cpp
)
add_library(${PROJECT_NAME} SHARED ${ENGINE_SOURCES})