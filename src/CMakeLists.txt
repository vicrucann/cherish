## Project settings
project(dureu3d)
cmake_minimum_required(VERSION 2.8.11)

## Compiler settings
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()
IF( CMAKE_COMPILER_IS_GNUCXX )
    ADD_DEFINITIONS(
        -Wall -Wundef -Wcast-align -Wchar-subscripts -Wpointer-arith
        -Wwrite-strings -Wpacked -Wformat-security
        -Wmissing-format-attribute -Wold-style-cast
    )
    ADD_DEFINITIONS( -fvisibility=hidden )
ELSEIF( CMAKE_CXX_COMPILER MATCHES "clang" )
    ADD_DEFINITIONS( -Wall -Wextra -Wno-unused-parameter )
    ADD_DEFINITIONS( -fvisibility=hidden )
ENDIF()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

## Required Qt packages
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
set(QT_LIBRARIES
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5OpenGL_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

## Required OSG packages
find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer osgText)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

## Test settings
find_package(Qt5Test REQUIRED)
enable_testing()
set(QT_QTTEST_LIBRARY
    ${Qt5Test_LIBRARIES}
)

## Icons directory setting
set(ICONS_DIR
    "${CMAKE_SOURCE_DIR}/icons"
)

## Adding subdirectories
add_subdirectory(dureu3d)
add_subdirectory(libGUI)
add_subdirectory(libSGEntities)
add_subdirectory(libSGControls)
add_subdirectory(libSGUtils)
add_subdirectory(tests)

## Installer settings (CPack) will be located here
