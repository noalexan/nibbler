cmake_minimum_required(VERSION 3.10)

project(nibbler VERSION 1.0 LANGUAGES CXX)

set(OpenGL_GL_PREFERENCE "LEGACY")

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include/)

add_library(core SHARED ${CMAKE_SOURCE_DIR}/src/Board.cpp ${CMAKE_SOURCE_DIR}/src/Snake.cpp)

set_target_properties(core PROPERTIES OUTPUT_NAME "nibbler")

add_executable(nibbler ${CMAKE_SOURCE_DIR}/src/main.cpp)

target_link_libraries(nibbler PRIVATE OpenGL::GL glfw core)

target_compile_features(nibbler PRIVATE cxx_std_17)

install(TARGETS core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(TARGETS nibbler
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
)
