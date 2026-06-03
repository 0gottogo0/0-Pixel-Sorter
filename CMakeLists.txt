cmake_minimum_required(VERSION 3.16)
project(PixelSorter CXX)

set(CMAKE_CXX_Standard 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

add_executable(PixelSorter src/main.cpp)

target_link_libraries(PixelSorter raylib)