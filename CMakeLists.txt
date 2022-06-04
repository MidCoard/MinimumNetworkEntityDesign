cmake_minimum_required(VERSION 3.17)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
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
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DWINDOWS)
    add_definitions(-D_WIN32_WINNT=0x600)
    link_libraries(ws2_32 wsock32)
else ()
    add_definitions(-DNON_WINDOWS)
endif ()

include_directories(layers)
include_directories(util)
include_directories(entities)
include_directories(packets)
include_directories(ui)
include_directories(.)

file(GLOB SOURCES "entities/*.*" "layers/*.*" "ui/*.*" "util/*.*" "util/network/*.*" "packets/*.*" "Network.h" "Network.cpp")

add_executable(NetworkDesign main.cpp ${SOURCES})

target_compile_options(NetworkDesign PUBLIC "-fno-omit-frame-pointer" "-mno-omit-leaf-frame-pointer")
target_link_libraries(NetworkDesign
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )