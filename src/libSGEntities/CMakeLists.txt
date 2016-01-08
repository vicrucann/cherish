if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libGUI
    ${CMAKE_SOURCE_DIR}/libSGUtils
    ${CMAKE_SOURCE_DIR}/libSGControls
)

set (libSGEntities_SRCS
    canvas.h
    canvas.cpp
    axes.h
    axes.cpp
    rootscene.h
    rootscene.cpp
    Stroke.h
    Stroke.cpp
    Photo.h
    Photo.cpp
)

add_library(libSGEntities
    STATIC ${libSGEntities_SRCS}
)

target_link_libraries( libSGEntities
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
