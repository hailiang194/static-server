cmake_minimum_required(VERSION 3.1)
project(static-server)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


include_directories(src)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vendors/mongoose")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vendors/mongoose")

add_executable(static-server src/main.c)

set_property(TARGET static-server PROPERTY C_STANDARD 99)

target_link_libraries(static-server PUBLIC mongoose)

