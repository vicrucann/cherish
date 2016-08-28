if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/cherish
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

set (libNumerics_SRCS
    PathFitter.h
    PathFitter.cpp

)

add_library(libNumerics
    STATIC ${libNumerics_SRCS}
)

target_link_libraries( libNumerics
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
