cmake_minimum_required(VERSION 3.22)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -O1")
project(NetworkDesign)

set(CMAKE_CXX_STANDARD 20)

include_directories(layers)
include_directories(util)
include_directories(entities)
include_directories(packets)
include_directories(.)

file(GLOB SOURCES "entities/*.*" "layers/*.*" "util/*.*" "util/network/*.*" "packets/*.*" "Network.h" "Network.cpp")

add_executable(NetworkDesign main.cpp ${SOURCES})

target_compile_options(NetworkDesign PUBLIC "-fno-omit-frame-pointer" "-mno-omit-leaf-frame-pointer")