project(dureu3d)
cmake_minimum_required(VERSION 2.8.11)

if(WIN32)
    if(MSVC)
        add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    endif(MSVC)
else(WIN32)
    set(CMAKE_CXX_FLAGS "-W -Wall -Wno-unused -std=c++0x")
endif(WIN32)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5OpenGL)
find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer)

include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

add_subdirectory(dureu3d)
add_subdirectory(libGUI)
add_subdirectory(libSG)
add_subdirectory(libSGControls)
