project(demo-qtosg)
cmake_minimum_required(VERSION 2.8.11)

if(WIN32)
    if(MSVC)
        add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    endif(MSVC)
else(WIN32)
    set(CMAKE_CXX_FLAGS "-W -Wall -Wno-unused")
endif(WIN32)
#SET( CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic -Wno-long-long" )

set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )

find_package( Qt5Core )
find_package( Qt5Gui  )
find_package( Qt5OpenGL )
find_package( OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer )

include_directories( ${OPENSCENEGRAPH_INCLUDE_DIRS} )

set( SOURCES
    main.cpp
    osgwidget.h
    osgwidget.cpp
    tabletapplication.h
    tabletapplication.cpp
    mainwindow.h
    mainwindow.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries( ${PROJECT_NAME}
  ${OPENSCENEGRAPH_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::OpenGL
)
