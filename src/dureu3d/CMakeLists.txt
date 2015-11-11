project(dureu3d)
cmake_minimum_required(VERSION 2.8.11)

include_directories(${CMAKE_SOURCE_DIR}/libGUI ${CMAKE_SOURCE_DIR}/libSG)

set(SOURCES
    main.cpp
    settings.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries( ${PROJECT_NAME}
  ${OPENSCENEGRAPH_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::OpenGL
  # defined within dureu3d
  libGUI
  libSG
)
