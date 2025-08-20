cmake_minimum_required(VERSION 3.19)

project(game_exe)

list(APPEND EXE_SOURCES
   "main.cpp"
)

add_executable(game WIN32 ${EXE_SOURCES})