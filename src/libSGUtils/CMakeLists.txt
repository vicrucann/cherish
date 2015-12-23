if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

set (libSGUtils_SRCS
    logfilehandler.h
    observescenecallback.h
    observescenecallback.cpp
    findnodevisitor.h
    findnodevisitor.cpp
)

add_library(libSGUtils
    STATIC ${libSGUtils_SRCS}
)

target_link_libraries( libSGUtils
    ${OPENSCENEGRAPH_LIBRARIES}
)
