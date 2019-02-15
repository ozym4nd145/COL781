cmake_minimum_required(VERSION 2.8.12)
project(Ray_Tracer)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(Eigen3 REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
include_directories(include/ include_lib/)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_STANDARD} -O3 -std=c++17")
FILE(GLOB SRCFILES src/*.cpp)
add_executable(ray_tracer ${SRCFILES})

target_link_libraries(ray_tracer ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
