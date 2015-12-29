if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libSGEntities
    ${CMAKE_SOURCE_DIR}/libSGControls
    ${CMAKE_SOURCE_DIR}/libSGUtils
)

set(libGUI_SRCS
    dureuapplication.cpp
    dureuapplication.h
    mainwindow.cpp
    mainwindow.h
    bookmarkwidget.h
    bookmarkwidget.cpp
    hudcamera.h
    listwidget.h
    listwidget.cpp
    glwidget.h
    glwidget.cpp
    canvasinfomodel.h
    canvasinfomodel.cpp
)

add_library(libGUI
    STATIC ${libGUI_SRCS}
)

target_link_libraries( libGUI
    ${QT_LIBRARIES}
)
