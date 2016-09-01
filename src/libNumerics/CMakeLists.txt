if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/cherish
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

add_subdirectory(CurveFitting/libPathFitter)

link_directories(${CMAKE_BINARY_DIR}/src/libNumerics/CurveFitting/libPathFitter)

set (libNumerics_SRCS
)

add_library(libNumerics
    STATIC ${libNumerics_SRCS}
)

target_link_libraries( libNumerics
    libPathFitter
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)
