cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (Particle)

#  IF(NOT CMAKE_BUILD_TYPE)
  #  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
#  ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

link_directories(${CMAKE_SOURCE_DIR}/lib)

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)

# note that the order is important for setting the libs
# use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread ${ASSIMP_LIBRARY})

set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

# first create relevant static libraries requried for other projects
add_library(STB_IMAGE "src/libs/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "src/libs/glad.c")
set(LIBS ${LIBS} GLAD)

FILE(GLOB COMMON_SRC ${CMAKE_SOURCE_DIR}/src/common/*.cpp)
add_library(app_lib ${COMMON_SRC})

set(LIBS ${LIBS} app_lib)
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})
set(LIBS ${LIBS} ${X11_LIBRARIES})
set(LIBS ${LIBS} m)
set(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT})


include_directories(${CMAKE_SOURCE_DIR}/include_lib ${CMAKE_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_STANDARD} -O3 -w -std=c++17")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_STANDARD} -g -w -std=c++17")


FILE(GLOB GENESIS_SRC src/genesis/*.cpp)
add_executable(genesis ${GENESIS_SRC})
target_link_libraries(genesis ${LIBS})

FILE(GLOB HOLI_SRC src/holi/*.cpp)
add_executable(holi ${HOLI_SRC})
target_link_libraries(holi ${LIBS})
