cmake_minimum_required(VERSION 2.8.11)

find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer)
include_directories(
    ${OPENSCENEGRAPH_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libSG
)


set (SOURCES_SGControls
    fixedviewhandler.h
    fixedviewhandler.cpp
    eventhandler.h
    eventhandler.cpp
    basehandler.h
    basehandler.cpp
    basemanipulator.h
    basemanipulator.cpp
    zoommanipulator.h
    rotatemanipulator.h
    manipulator.h
    manipulator.cpp
)

add_library(libSGControls ${SOURCES_SGControls})

target_link_libraries( libSGControls
	${OPENSCENEGRAPH_LIBRARIES}
)
