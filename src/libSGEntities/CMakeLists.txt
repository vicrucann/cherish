if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/cherish
    ${CMAKE_SOURCE_DIR}/libGUI
    ${CMAKE_SOURCE_DIR}/libSGControls
    ${CMAKE_SOURCE_DIR}/libNumerics
)

set (libSGEntities_SRCS
    ProtectedGroup.h
    Canvas.h
    Canvas.cpp
    RootScene.h
    RootScene.cpp
    Stroke.h
    Stroke.cpp
    Photo.h
    Photo.cpp
    UserScene.h
    UserScene.cpp
    Entity2D.h
    Entity2D.cpp
    ShaderedEntity2D.h
    ShaderedEntity2D.cpp
    Polygon.h
    Polygon.cpp
    ToolGlobal.h
    ToolGlobal.cpp
    Bookmarks.h
    Bookmarks.cpp
    SelectedGroup.h
    SelectedGroup.cpp
    SceneState.h
    SceneState.cpp
    SVMData.h
    SVMData.cpp
    CamPoseData.h
    CamPoseData.cpp
    DraggableWire.h
    DraggableWire.cpp
    EditableWire.h
    EditableWire.cpp
)

add_library(libSGEntities
    STATIC ${libSGEntities_SRCS}
)

target_link_libraries( libSGEntities
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
