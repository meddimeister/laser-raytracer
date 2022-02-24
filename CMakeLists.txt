cmake_minimum_required(VERSION 3.0)

set(PROG_NAME laser-raytracer)
project (${PROG_NAME})

############# Required LIBRARIES ###############

include_directories(include)
include_directories(lib/glm/glm)

add_subdirectory(lib/nomad)
include_directories(lib/nomad/src)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/nomad/src)

############# COMPILER FLAGS ###############

############# SOURCE ###############

FILE(GLOB_RECURSE PROG_SRC  src/*.cpp)
FILE(GLOB_RECURSE PROG_HEADER  include/*.h)

add_executable(${PROG_NAME} ${PROG_SRC} ${PROG_HEADER})
target_link_libraries(${PROG_NAME} ${LIBS} nomadAlgos nomadEval nomadUtils)
target_compile_features(${PROG_NAME} PUBLIC cxx_std_17)

add_custom_command(
        TARGET ${PROG_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/scripts/plot.plt
                $<TARGET_FILE_DIR:${PROG_NAME}>)