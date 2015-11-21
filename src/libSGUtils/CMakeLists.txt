cmake_minimum_required(VERSION 2.8.11)

find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer)
include_directories(
    ${OPENSCENEGRAPH_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

set (SOURCES_SGUtils
    logfilehandler.h
    observescenecallback.h
    observescenecallback.cpp
    findnodevisitor.h
    findnodevisitor.cpp
)

add_library(libSGUtils ${SOURCES_SGUtils})

target_link_libraries( libSGUtils
	${OPENSCENEGRAPH_LIBRARIES}
)
