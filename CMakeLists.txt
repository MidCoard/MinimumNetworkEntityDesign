cmake_minimum_required(VERSION 3.22)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -O0")
project(NetworkDesign)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

include_directories(layers)
include_directories(util)
include_directories(entities)
include_directories(packets)
include_directories(.)

file(GLOB SOURCES "entities/*.*" "layers/*.*" "util/*.*" "util/network/*.*" "packets/*.*" "Network.h" "Network.cpp")

add_executable(NetworkDesign main.cpp ${SOURCES})

target_compile_options(NetworkDesign PUBLIC "-fno-omit-frame-pointer" "-mno-omit-leaf-frame-pointer")
target_link_libraries(NetworkDesign
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )