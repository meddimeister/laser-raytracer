cmake_minimum_required(VERSION 2.8)

set(PROG_NAME laser-raytracer)
project (${PROG_NAME})

############# Required LIBRARIES ###############

############# COMPILER FLAGS ###############

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

############# SOURCE ###############

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB_RECURSE PROG_SRC  src/*.cpp)
FILE(GLOB_RECURSE PROG_HEADER  src/*.h)

add_executable(${PROG_NAME} ${PROG_SRC} ${PROG_HEADER})
target_link_libraries(${PROG_NAME} ${LIBS} )
