## Project settings
project(cherish)
cmake_minimum_required(VERSION 2.8.11)


## Build options
option(cheris_BUILD_DOC "Build cherish documentation" ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
message(STATUS "Building in ${CMAKE_BUILD_TYPE} mode")


## Compiler settings
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()
if( CMAKE_COMPILER_IS_GNUCXX )
    ADD_DEFINITIONS(
        -Wall -Wundef -Wcast-align -Wchar-subscripts -Wpointer-arith
        -Wwrite-strings -Wpacked -Wformat-security
        -Wmissing-format-attribute -Wold-style-cast
    )
    ADD_DEFINITIONS( -fvisibility=hidden )
elseif( CMAKE_CXX_COMPILER MATCHES "clang" )
    ADD_DEFINITIONS( -Wall -Wextra -Wno-unused-parameter )
    ADD_DEFINITIONS( -fvisibility=hidden )
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if( CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$" )
    add_definitions( -DQT_NO_DEBUG_OUTPUT )
endif()
add_definitions( -DQT_COMPILING_QSTRING_COMPAT_CPP)


## Required Qt packages
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5Xml REQUIRED)
set(QT_LIBRARIES
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5OpenGL_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
)


## Required OSG packages
find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer osgText)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})


## Other build packages
if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")
        add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
      message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif()


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
add_subdirectory(cherish)
add_subdirectory(libGUI)
add_subdirectory(libSGEntities)
add_subdirectory(libSGControls)
add_subdirectory(libSGUtils)
add_subdirectory(tests)


## Installer settings (CPack) will be located here
