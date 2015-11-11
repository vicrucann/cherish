cmake_minimum_required(VERSION 2.8.11)

find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

include_directories(
  ${CMAKE_SOURCE_DIR}/libGUI
  ${CMAKE_SOURCE_DIR}/dureu3d)

set (SOURCES_SG
    #scenegraph.h
    #scenegraph.cpp
    canvas.h
    canvas.cpp
    axes.h
    axes.cpp
)

add_library(libSG ${SOURCES_SG})

target_link_libraries( libSG
  ${OPENSCENEGRAPH_LIBRARIES}
)
