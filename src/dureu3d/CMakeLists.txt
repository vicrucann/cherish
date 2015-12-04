project(dureu3d)
cmake_minimum_required(VERSION 2.8.11)

include_directories(
    ${CMAKE_SOURCE_DIR}/libGUI
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

set(SOURCES
    main.cpp
    settings.h
)

qt5_add_resources(IMG_RSC_ADDED icons/image.qrc)
add_executable(${PROJECT_NAME} ${SOURCES} ${IMG_RSC_ADDED})

target_link_libraries( ${PROJECT_NAME}
    # dynamic
    ${OPENSCENEGRAPH_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL

    # defined within dureu3d
    libGUI
    libSGEntities
    libSGControls
    libSGUtils
)
