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

add_subdirectory(CurveFitting/libPathFitter)

link_directories(${CMAKE_BINARY_DIR}/src/libNumerics/CurveFitting/libPathFitter)

set (libNumerics_SRCS
    HomographyMatrix.h
    HomographyMatrix.cpp
    libHomogrpahy/matrix.cpp
    libHomogrpahy/matrix.h
    libHomogrpahy/vector.cpp
    libHomogrpahy/Hmatrix.h
    libHomogrpahy/SVD.h
    libHomogrpahy/ccmath_svd.cpp
    libHomogrpahy/LM.h
    libHomogrpahy/alternatives.h
    )

add_library(libNumerics STATIC
    ${libNumerics_SRCS}
    )

target_link_libraries( libNumerics
    libPathFitter
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
    #${OpenCV_LIBS}
)
