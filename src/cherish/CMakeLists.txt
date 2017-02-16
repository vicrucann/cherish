include_directories(
    ${CMAKE_SOURCE_DIR}/libGUI
    ${CMAKE_SOURCE_DIR}/libSGEntities
    ${CMAKE_SOURCE_DIR}/libNumerics
    ${CMAKE_SOURCE_DIR}/tests
)

if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

set(CherishApplication_SRCS
    main.cpp
    Settings.h
    Data.h
    Data.cpp
    Utilities.h
    Utilities.cpp
)

qt5_add_resources(IMG_RSC_ADDED
    Images/Actions.qrc
    Images/Icons.qrc
    Images/Logo.qrc
    Styles/Stylesheets.qrc
)

file(GLOB_RECURSE SHADER_RSC_ADDED
    Shaders/*.frag
    Shaders/*.vert
    Shaders/*.geom
)

add_executable(${PROJECT_NAME}
    ${CherishApplication_SRCS}
    ${IMG_RSC_ADDED}
    ${SHADER_RSC_ADDED}
)

target_link_libraries( ${PROJECT_NAME}
    # dynamic
    ${OPENSCENEGRAPH_LIBRARIES}
    ${QT_LIBRARIES}
    #${OpenCV_LIBS}

    # defined within cherish
    libGUI
    libSGEntities
    libSGControls
    libNumerics
)

## copy shader files to binary and test folders
foreach(shaderpath ${SHADER_RSC_ADDED})
    get_filename_component(shader ${shaderpath} NAME)
    configure_file("${CMAKE_SOURCE_DIR}/cherish/Shaders/${shader}" "${CMAKE_BINARY_DIR}/cherish/Shaders/${shader}" COPYONLY)
    configure_file("${CMAKE_SOURCE_DIR}/cherish/Shaders/${shader}" "${CMAKE_BINARY_DIR}/tests/Shaders/${shader}" COPYONLY)
endforeach()

if( CMAKE_BUILD_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])" )
        set( CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE )
endif()
