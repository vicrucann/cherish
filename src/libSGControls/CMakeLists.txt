cmake_minimum_required(VERSION 2.8.11)

find_package(OpenSceneGraph REQUIRED COMPONENTS osgGA osgUtil osgViewer)
include_directories(
    ${OPENSCENEGRAPH_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libSGEntities
)


set (SOURCES_SGControls
    eventhandler.h
    eventhandler.cpp
    manipulator.h
    manipulator.cpp
)

add_library(libSGControls ${SOURCES_SGControls})

target_link_libraries( libSGControls
	${OPENSCENEGRAPH_LIBRARIES}
)
