cmake_minimum_required(VERSION 3.10)

project(SDL2_nibbler_frontend VERSION 1.0 LANGUAGES CXX)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(SDL2_nibbler SHARED ${CMAKE_SOURCE_DIR}/src/SDL2.cpp)

target_include_directories(SDL2_nibbler PRIVATE ${CMAKE_SOURCE_DIR}/lib/nibbler/include)

target_link_libraries(SDL2_nibbler ${SDL2_LIBRARIES})
set_target_properties(SDL2_nibbler PROPERTIES PREFIX "")
