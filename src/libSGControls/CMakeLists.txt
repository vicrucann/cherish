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
    Manipulator.h
    Manipulator.cpp
    AddEntityCommand.h
    AddEntityCommand.cpp
    EditEntityCommand.h
    EditEntityCommand.cpp
    StrokeIntersector.h
    StrokeIntersector.cpp
)

add_library(libSGControls
    STATIC ${libSGControls_SRCS}
)

target_link_libraries( libSGControls
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
