include_directories(
    ${CMAKE_SOURCE_DIR}/libGUI
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

set(DureuApplication_SRCS
    main.cpp
    settings.h
    Data.h
    Data.cpp
)

qt5_add_resources(IMG_RSC_ADDED
    icons/image.qrc
    Data/Actions.qrc
)

add_executable(${PROJECT_NAME}
    ${DureuApplication_SRCS}
    ${IMG_RSC_ADDED}
)

target_link_libraries( ${PROJECT_NAME}
    # dynamic
    ${OPENSCENEGRAPH_LIBRARIES}
    ${QT_LIBRARIES}

    # defined within dureu3d
    libGUI
    libSGEntities
    libSGControls
    libSGUtils
)

if( CMAKE_BUILD_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])" )
        set( CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE )
endif()
