cmake_minimum_required(VERSION 3.29.2)
project(Soundboard)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_CXX_STANDARD 20)

add_executable(Soundboard src/main.cpp src/validateFiles.cpp src/utils.cpp src/sound.cpp src/globals.cpp)
