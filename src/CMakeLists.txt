## Project settings
project(Cherish)
cmake_minimum_required(VERSION 2.8.11)

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()


## Project version
if (NOT Cherish_VERSION)
    find_package(Git QUIET)
    if (EXISTS ${GIT_EXECUTABLE})
        execute_process( COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=1
            RESULT_VARIABLE GIT_RESULT
            OUTPUT_VARIABLE Cherish_VERSION
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_STRIP_TRAILING_WHITESPACE )
        if (NOT GIT_RESULT EQUAL 0)
            message(FATAL_ERROR "Cannot get 'git describe' version.")
        endif()
    endif()
endif()

string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.(.*)$" Cherish_VERSION "${Cherish_VERSION}")
set(Cherish_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(Cherish_VERSION_MINOR "${CMAKE_MATCH_2}")
set(Cherish_VERSION_PATCH "${CMAKE_MATCH_3}")
set(Cherish_VERSION_COUNT 3)

if (NOT ( DEFINED Cherish_VERSION_MAJOR AND DEFINED Cherish_VERSION_MINOR AND DEFINED Cherish_VERSION_PATCH ))
    message(FATAL_ERROR "No Git executable or valid Cherish version argument found.\n"
        "Please pass a version to CMake e.g. cmake -DCherish_VERSION=1.0.0")
endif()

## Do not allow in-source builds
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds are not allowed")
endif()


## Build options
option(Cherish_BUILD_TESTS "Build Cherish tests" ON)
option(cheris_BUILD_DOC "Build Cherish documentation (requires Doxygen installed)" OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()
message( STATUS "Building Cherish ${Cherish_VERSION} in ${CMAKE_BUILD_TYPE} mode" )


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
find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Gui 5.4 REQUIRED)
find_package(Qt5OpenGL 5.4 REQUIRED)
find_package(Qt5Widgets 5.4 REQUIRED)
find_package(Qt5Svg 5.4 REQUIRED)
find_package(Qt5Xml 5.4 REQUIRED)
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
#if (cheris_BUILD_DOC)
if(cheris_BUILD_DOC)
    message("Trying to build documentation")
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
endif(cheris_BUILD_DOC)


## Test settings
if (Cherish_BUILD_TESTS)
    find_package(Qt5Test 5.4 REQUIRED)
    enable_testing()
    set(QT_QTTEST_LIBRARY
        ${Qt5Test_LIBRARIES}
        )
endif()

## Adding subdirectories
add_subdirectory(cherish)
add_subdirectory(libGUI)
add_subdirectory(libSGEntities)
add_subdirectory(libSGControls)
add_subdirectory(libNumerics)
if (Cherish_BUILD_TESTS)
    add_subdirectory(tests)
endif()

## copy shaders (only runs on cmake, not make command)
file (COPY ${CMAKE_SOURCE_DIR}/cherish/Shaders DESTINATION ${CMAKE_BINARY_DIR}/cherish)
file (COPY ${CMAKE_SOURCE_DIR}/cherish/Shaders DESTINATION ${CMAKE_BINARY_DIR}/tests)


## Installer settings (CPack) will be located here
