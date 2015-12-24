if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

set (libSGControls_SRCS
    eventhandler.h
    eventhandler.cpp
    manipulator.h
    manipulator.cpp
)

add_library(libSGControls
    STATIC ${libSGControls_SRCS}
)

target_link_libraries( libSGControls
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
