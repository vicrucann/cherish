cmake_minimum_required(VERSION 2.8.11)

include_directories(
    ${cherish_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/cherish
    ${CMAKE_SOURCE_DIR}/libGUI
    ${CMAKE_SOURCE_DIR}/libSGEntities
    ${CMAKE_SOURCE_DIR}/libSGControls
)

if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

SET( TEST_LIBRARIES
    libSGEntities
    libSGControls
    libGUI
    ${QT_QTTEST_LIBRARY}
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)

qt5_add_resources(IMG_RSC_ADDED
    ${CMAKE_SOURCE_DIR}/cherish/Images/Actions.qrc
    ${CMAKE_SOURCE_DIR}/cherish/Images/Icons.qrc
)

# base test to inherit, which includes running MainWindow
set(BASETEST_SRC
    BaseTest.h
    BaseTest.cpp
    ${CMAKE_SOURCE_DIR}/cherish/Data.h
    ${CMAKE_SOURCE_DIR}/cherish/Data.cpp
    ${CMAKE_SOURCE_DIR}/cherish/Utilities.h
    ${CMAKE_SOURCE_DIR}/cherish/Utilities.cpp
    ${CMAKE_SOURCE_DIR}/cherish/Settings.h
    )

set(BASETEST_NAME base_test)
add_executable(${BASETEST_NAME} ${BASETEST_SRC})
target_link_libraries(${BASETEST_NAME} ${TEST_LIBRARIES})
add_test(${BASETEST_NAME} ${BASETEST_SRC})

# SceneState tests with relation to bookmarks (SG and GUI) and canvas-photo widget (GUI)
#set(SCENESTATE_SRC SceneStateTest.cpp)
#set(SCENESTATE_NAME scenestate_test)
#add_executable(${SCENESTATE_NAME} ${SCENESTATE_SRC})
#target_link_libraries(${SCENESTATE_NAME} ${TEST_LIBRARIES})
#add_test(${SCENESTATE_NAME} ${SCENESTATE_SRC})
