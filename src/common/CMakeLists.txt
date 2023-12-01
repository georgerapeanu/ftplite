cmake_minimum_required(VERSION 3.22)
project(ftplite)

set(target "common")
add_library(${target})
target_sources(${target} PRIVATE CommunicationWrapper.cpp)

add_subdirectory(connection)
add_subdirectory(modes)
add_subdirectory(types)
