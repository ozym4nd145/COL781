cmake_minimum_required(VERSION 2.8.12)
project(Ray_Tracer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_STANDARD} -O3 -std=c++17")

include_directories(include/ /usr/local/include/eigen3/)

FILE(GLOB SRCFILES src/*.cpp)

add_executable(ray_tracer ${SRCFILES})