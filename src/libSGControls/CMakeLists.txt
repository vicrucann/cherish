if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/cherish
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

set (libSGControls_SRCS
    EventHandler.h
    EventHandler.cpp
    Manipulator.h
    Manipulator.cpp
    AddEntityCommand.h
    AddEntityCommand.cpp
    EditEntityCommand.h
    EditEntityCommand.cpp
    StrokeIntersector.h
    StrokeIntersector.cpp
    LineIntersector.h
    LineIntersector.cpp
    ViewerCommand.h
    ViewerCommand.cpp
    LogFileHandler.h
    FindNodeVisitor.h
    FindNodeVisitor.cpp
    CameraCallbacks.h
    CameraCallbacks.cpp
    ProgramEntity2D.h
    ProgramEntity2D.cpp
    ProgramStroke.h
    ProgramStroke.cpp
    ProgramPolygon.h
    ProgramPolygon.cpp
)

add_library(libSGControls
    STATIC ${libSGControls_SRCS}
)

target_link_libraries( libSGControls
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
