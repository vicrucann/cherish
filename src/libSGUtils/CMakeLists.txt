if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

set (libSGUtils_SRCS
    LogFileHandler.h
    FindNodeVisitor.h
    FindNodeVisitor.cpp
    CameraChangeCallback.h
    CameraChangeCallback.cpp
)

add_library(libSGUtils
    STATIC ${libSGUtils_SRCS}
)

target_link_libraries( libSGUtils
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
