cmake_minimum_required(VERSION 2.8.11)
project(qtosg)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui OpenGL)

set(SOURCES
    qt.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    )
