cmake_minimum_required(VERSION 3.0)

project(snake3d)

add_subdirectory(my3d)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${PROJECT_SOURCE_DIR}/bin)

link_directories(${PROJECT_SOURCE_DIR}/bin/my3d)

aux_source_directory(. DIR_SRCS)

add_executable(snake3d ${DIR_SRCS})
target_link_libraries(snake3d my3d)