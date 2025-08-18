cmake_minimum_required(VERSION 3.10) 
project(alchemist)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED) 
find_package(SDL2_image REQUIRED) 
find_package(SDL2_ttf REQUIRED) 
include_directories(${SDL2_INCLUDE_DIRS} include)
include_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(alchemist src/main.cpp) 
target_link_libraries(alchemist SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)