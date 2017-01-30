cmake_minimum_required(VERSION 2.8.11)

if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()
if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

include_directories(${CMAKE_SOURCE_DIR})

set (libHomography_SRCS
    matrix.cpp 
    matrix.h 
    vector.cpp
    Hmatrix.h
    SVD.h
    )

add_library(libHomography
    STATIC ${libHomography_SRCS}
    )
