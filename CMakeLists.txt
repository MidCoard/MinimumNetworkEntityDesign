cmake_minimum_required(VERSION 3.22)
project(NetworkDesign)

set(CMAKE_CXX_STANDARD 14)

include_directories(layers)
include_directories(util)

set(NETWORK_CPP Network.h Network.cpp)

add_executable(NetworkDesign main.cpp layers util entities ${NETWORK_CPP})
