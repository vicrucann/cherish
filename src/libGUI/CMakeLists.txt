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
    DureuApplication.cpp
    DureuApplication.h
    mainwindow.cpp
    mainwindow.h
    hudcamera.h
    glwidget.h
    glwidget.cpp
)

qt5_wrap_ui(UI_GENERATED_SRCS
    Widgets/MainWindow.ui
)

add_library(libGUI STATIC
    ${libGUI_SRCS}
    ${UI_GENERATED_SRCS}
)

target_link_libraries( libGUI
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
