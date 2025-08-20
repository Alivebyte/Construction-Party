cmake_minimum_required(VERSION 3.29)

project(game_exe)

list(APPEND EXE_SOURCES
   "main.cpp"
)

add_executable(game ${EXE_SOURCES})