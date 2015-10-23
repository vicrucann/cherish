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

## OpenSceneGraph paths - external dependencies
# LINUX header files
find_path(OSG_INCLUDE_DIR
    PATHS
    #$ENV{OSG_INCLUDE_PATH} # windows does not work this way
    /usr/local/include  # linux or cygwin
    /usr/local/OpenSceneGraph/INCLUDE_DIRECTORIES
)

# LINUX library files
find_path(OSG_LIB_DIR
    PATHS
    #$ENV{OSG_LIB_PATH} # windows does not work this way
    /usr/local/lib # linux or cygwin
    /usr/local/lib64
    /usr/local/OpenSceneGraph/lib
)

## apply the paths to projects and include windows path
include_directories($ENV{OSG_INCLUDE_PATH} ${OSG_INCLUCE_DIR})
link_directories($ENV{OSG_LIB_PATH} ${OSG_LIB_DIR})

## sample datafiles
#configure_file(DATA_FILE ${PROJECT_SOURCE_DIR}/DATA_FILE COPYONLY)

# project's own files
set(PROJECT_NAME demo-osg)
#set(SRC_LIST main.cpp)
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} WIN32 ${SRC_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

# link the osg libs to the project
target_link_libraries(${PROJECT_NAME}
    debug osg${CMAKE_DEBUG_POSTFIX} optimized osg
    debug osgUtil${CMAKE_DEBUG_POSTFIX} optimized osgUtil
    debug osgViewer${CMAKE_DEBUG_POSTFIX} optimized osgViewer
    debug osgDB${CMAKE_DEBUG_POSTFIX} optimized osgDB
    debug osgGA${CMAKE_DEBUG_POSTFIX} optimized osgGA
    debug OpenThreads${CMAKE_DEBUG_POSTFIX} optimized OpenThreads
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
#    Qt5:Widgets
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
