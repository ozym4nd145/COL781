cmake_minimum_required(VERSION 2.8.12)
project(Ray_Tracer)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_STANDARD} -O3 -std=c++17")
include_directories(include/)

FILE(GLOB SRCFILES src/*.cpp)

add_executable(ray_tracer ${SRCFILES})
