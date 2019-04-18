cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (Bowling)

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

# find_library(IrrKlang_LIB IrrKlang ${CMAKE_SOURCE_DIR}/lib)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)
# note that the order is important for setting the libs
# use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread ${ASSIMP_LIBRARY})

set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

# first create relevant static libraries requried for other projects
add_library(STB_IMAGE "src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

include_directories(${CMAKE_SOURCE_DIR}/include_lib ${CMAKE_SOURCE_DIR}/include)
# include_directories(${CMAKE_SOURCE_DIR}/include_lib/irrKlang)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_STANDARD} -O3 -std=c++17")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_STANDARD} -g -std=c++17")
FILE(GLOB SRCFILES src/*.cpp src/*.c)
add_executable(holi ${SRCFILES} )

target_link_libraries(holi ${LIBS})
target_link_libraries(holi ${OPENGL_LIBRARIES})
target_link_libraries(holi ${X11_LIBRARIES} m)
target_link_libraries(holi ${CMAKE_THREAD_LIBS_INIT})
# target_link_libraries(holi ${IrrKlang_LIB})

