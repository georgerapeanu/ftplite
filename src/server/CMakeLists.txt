cmake_minimum_required(VERSION 3.22)
project(ftplite)

set(target "server")
target_sources(${target} PRIVATE main.cpp)
target_link_libraries(${target} PRIVATE common)
