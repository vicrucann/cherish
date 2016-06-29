if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/cherish
    ${CMAKE_SOURCE_DIR}/libSGEntities
    ${CMAKE_SOURCE_DIR}/libSGControls
)

set(libGUI_SRCS
    CherishApplication.cpp
    CherishApplication.h
    MainWindow.cpp
    MainWindow.h
    GLWidget.h
    GLWidget.cpp
    ListWidget.h
    ListWidget.cpp
    TreeWidget.h
    TreeWidget.cpp
    ListDelegate.h
    ListDelegate.cpp
    CameraProperties.h
    CameraProperties.cpp
)

qt5_wrap_ui(UI_GENERATED_SRCS
    Widgets/MainWindowUI.ui
    Widgets/QtOSGWindow.ui
    Widgets/CameraProperties.ui
)

add_library(libGUI STATIC
    ${libGUI_SRCS}
    ${UI_GENERATED_SRCS}
)

target_link_libraries( libGUI
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
