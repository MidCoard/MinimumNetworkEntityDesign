cmake_minimum_required(VERSION 3.22)
project(NetworkDesign)

set(CMAKE_CXX_STANDARD 20)

include_directories(layers)
include_directories(util)
include_directories(entities)
include_directories(.)

file(GLOB SOURCES "entities/*.*" "layers/*.*" "util/*.*" "util/network/*.*" "Network.h" "Network.cpp")

add_executable(NetworkDesign main.cpp ${SOURCES})
