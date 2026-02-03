cmake_minimum_required(VERSION 3.10)

project(OpenGL_nibbler_frontend VERSION 1.0 LANGUAGES CXX)

set(OpenGL_GL_PREFERENCE "LEGACY")

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_library(OpenGL_nibbler SHARED ${CMAKE_SOURCE_DIR}/src/OpenGL.cpp)

target_include_directories(OpenGL_nibbler PRIVATE ${CMAKE_SOURCE_DIR}/lib/nibbler/include)

target_link_libraries(OpenGL_nibbler PRIVATE OpenGL::GL glfw)

target_compile_features(OpenGL_nibbler PRIVATE cxx_std_17)
set_target_properties(OpenGL_nibbler PROPERTIES PREFIX "" LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
