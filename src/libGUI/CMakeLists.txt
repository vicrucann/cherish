cmake_minimum_required(VERSION 2.8.11)

include_directories(
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libSGEntities
    ${CMAKE_SOURCE_DIR}/libSGControls
    ${CMAKE_SOURCE_DIR}/libSGUtils
)

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5OpenGL)

set(SOURCES_GUI
    dureuapplication.cpp
    dureuapplication.h
    mainwindow.cpp
    mainwindow.h
    viewwidget.cpp
    viewwidget.h
    bookmarkwidget.h
    bookmarkwidget.cpp
)

add_library(libGUI ${SOURCES_GUI})

target_link_libraries( libGUI
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
)
