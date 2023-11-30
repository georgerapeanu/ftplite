cmake_minimum_required(VERSION 3.22)
project(ftplite)

message(${target})
target_sources(${target} PRIVATE  AbstractConnection.cpp 
                                  ActiveStartedConnection.cpp 
                                  PasiveStartedConnection.cpp 
                                  TCPListener.cpp
)
