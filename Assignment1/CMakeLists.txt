cmake_minimum_required(VERSION 2.8.12)
project(Ray_Tracer)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(Eigen3 REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

# find the required packages
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)


set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread ${ASSIMP_LIBRARY})
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
set(CMAKE_BUILD_TYPE Debug)

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
include_directories(${X11_INCLUDE_DIR})

include_directories(include/ include_lib/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_STANDARD} -std=c++17")
FILE(GLOB SRCFILES src/*.cpp src/*.c)
add_executable(ray_tracer ${SRCFILES} )

target_link_libraries(ray_tracer ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
target_link_libraries(ray_tracer ${X11_LIBRARIES} m)
target_link_libraries(ray_tracer ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ray_tracer ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${LIBS})
