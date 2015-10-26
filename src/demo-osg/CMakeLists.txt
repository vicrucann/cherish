## General headers
cmake_minimum_required(VERSION 2.8.11)

project(demo-osg)

## cmake settings
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix for Debug mode, usually d on windows")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

## cmake policy (for lib search)
if (COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
    CMAKE_POLICY(SET CMP0015 NEW)
endif(COMMAND CMAKE_POLICY)

## C++ compile flags
if(WIN32)
    if(MSVC)
        add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    endif(MSVC)
else(WIN32)
    set(CMAKE_CXX_FLAGS "-W -Wall -Wno-unused")
endif(WIN32)

## Qt5 used packages
#find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5OpenGL)
find_package(OpenThreads)
find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer)

INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} )

# project's own files
set(PROJECT_NAME demo-osg)
#set(SRC_LIST main.cpp)
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} WIN32 ${SRC_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
  ${OPENSCENEGRAPH_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::OpenGL
)
