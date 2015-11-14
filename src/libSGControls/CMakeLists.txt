cmake_minimum_required(VERSION 2.8.11)

find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

set (SOURCES_SGControls
    fixedviewhandler.h
    fixedviewhandler.cpp
)

add_library(libSGControls ${SOURCES_SGControls})

target_link_libraries( libSGControls
	${OPENSCENEGRAPH_LIBRARIES}
)
