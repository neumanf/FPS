cmake_minimum_required (VERSION 2.8)
project (FPS)
set(VERSION "0.1.0")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
file(GLOB_RECURSE FPS_SOURCES "src/*.cpp")

add_executable(FPS ${FPS_SOURCES})

set(CMAKE_CXX_FLAGS "-std=c++11")
target_link_libraries(FPS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})