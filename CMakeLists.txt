cmake_minimum_required (VERSION 3.12)
project (farm)
set(VERSION "0.1.0")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
file(GLOB_RECURSE FARM_SOURCES "src/*.cpp")

add_executable(farm ${FARM_SOURCES})

set(CMAKE_CXX_FLAGS "-std=c++11")
target_link_libraries(farm ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})