cmake_minimum_required(VERSION 3.22)
project(NetworkDesign)

set(CMAKE_CXX_STANDARD 14)

include_directories(layers)

add_executable(NetworkDesign main.cpp layers)
