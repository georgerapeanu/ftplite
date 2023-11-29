
cmake_minimum_required(VERSION 3.22)
project(ftplite)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target/lib)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(client)
add_executable(server)

include_directories(include)
add_subdirectory(include)
add_subdirectory(src)


