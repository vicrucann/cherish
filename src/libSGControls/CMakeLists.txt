if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

set (libSGControls_SRCS
    EventHandler.h
    EventHandler.cpp
    manipulator.h
    manipulator.cpp
    AddStrokeCommand.h
    AddStrokeCommand.cpp
    DeleteStrokeCommand.h
    DeleteStrokeCommand.cpp
    AddCanvasCommand.h
    AddCanvasCommand.cpp
    AddPhotoCommand.h
    AddPhotoCommand.cpp
    EditEntityCommand.h
    EditEntityCommand.cpp
)

add_library(libSGControls
    STATIC ${libSGControls_SRCS}
)

target_link_libraries( libSGControls
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
