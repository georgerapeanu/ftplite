cmake_minimum_required(VERSION 3.22)
project(ftplite)

add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(common)

target_sources(common PRIVATE AppException.cpp)
target_sources(client PRIVATE AppException.cpp)
target_sources(server PRIVATE AppException.cpp)

