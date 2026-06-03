cmake_minimum_required(VERSION 3.16)
project(PixelSorter C CXX)

set(CMAKE_CXX_Standard 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

add_executable(PixelSorter src/main.cpp external/tinyfiledialogs.c)


target_include_directories(PixelSorter PRIVATE
    external
)

target_link_libraries(PixelSorter raylib)