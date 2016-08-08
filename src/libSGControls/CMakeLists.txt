if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
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
    TestSupportOperation.h
    TestSupportOperation.cpp
)

add_library(libSGControls
    STATIC ${libSGControls_SRCS}
)

target_link_libraries( libSGControls
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
