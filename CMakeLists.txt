cmake_minimum_required(VERSION 3.10)

set(PROG_NAME laser-raytracer)
project (${PROG_NAME})

############# Required LIBRARIES ###############

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(lib/glm/glm)

#add_subdirectory(lib/nomad)
include_directories(lib/nomad3/src)
include_directories(lib/nomad3/ext/sgtelib/src)
#link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/nomad/src)
link_directories(lib/nomad3/lib)

############# COMPILER FLAGS ###############
#set(CMAKE_CXX_FLAGS -pg)
#set(CMAKE_EXE_LINKER_FLAGS -pg)
#set(CMAKE_SHARED_LINKER_FLAGS -pg)

############# SOURCE ###############

FILE(GLOB_RECURSE PROG_SRC  src/**/*.cpp)
FILE(GLOB_RECURSE PROG_HEADER  src/**/*.h)

add_executable(${PROG_NAME} src/main.cpp ${PROG_SRC} ${PROG_HEADER})
add_executable(${PROG_NAME}-plots src/plots.cpp ${PROG_SRC} ${PROG_HEADER})
add_executable(${PROG_NAME}-experiment src/experiment.cpp ${PROG_SRC} ${PROG_HEADER})
target_link_libraries(${PROG_NAME} ${LIBS} nomad)
target_compile_features(${PROG_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROG_NAME}-plots ${LIBS} nomad)
target_compile_features(${PROG_NAME}-plots PUBLIC cxx_std_17)
target_link_libraries(${PROG_NAME}-experiment ${LIBS} nomad)
target_compile_features(${PROG_NAME}-experiment PUBLIC cxx_std_17)