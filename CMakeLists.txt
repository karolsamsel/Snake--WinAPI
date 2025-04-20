cmake_minimum_required(VERSION 3.10)

# Project name
project(Snake)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Explicitly define compilers (if needed)
set(CMAKE_C_COMPILER "C:/Program Files (x86)/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/mingw64/bin/g++.exe")


file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/game/src/*.cpp")
file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/platform/src/*.cpp")


add_executable(main ${GAME_SOURCES} ${PLATFORM_SOURCES})
target_include_directories(main PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/game/include") 
target_include_directories(main PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/platform/include") 