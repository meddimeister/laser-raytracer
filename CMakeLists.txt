cmake_minimum_required(VERSION 2.8)

set(PROG_NAME laser-raytracer)
project (${PROG_NAME})

############# Required LIBRARIES ###############

############# COMPILER FLAGS ###############

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")


############# SOURCE ###############

include_directories(include)
include_directories(lib/glm/glm)

FILE(GLOB_RECURSE PROG_SRC  src/*.cpp)
FILE(GLOB_RECURSE PROG_HEADER  include/*.h)

add_executable(${PROG_NAME} ${PROG_SRC} ${PROG_HEADER})
target_link_libraries(${PROG_NAME} ${LIBS} )
target_compile_features(${PROG_NAME} PUBLIC cxx_std_17)

add_custom_command(
        TARGET ${PROG_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/scripts/plot.plt
                $<TARGET_FILE_DIR:${PROG_NAME}>)