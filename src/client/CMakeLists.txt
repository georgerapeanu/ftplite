cmake_minimum_required(VERSION 3.22)
project(ftplite)

set(target "client")
target_sources(${target} PRIVATE main.cpp test.cpp)
target_link_libraries(${target} PRIVATE common)
