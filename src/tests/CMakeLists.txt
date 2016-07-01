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

## load resources such as icons for actions
qt5_add_resources(IMAGE_RSC
    ${CMAKE_SOURCE_DIR}/cherish/Images/Actions.qrc
    ${CMAKE_SOURCE_DIR}/cherish/Images/Icons.qrc
)

## set of base files that every test will, probably, need
set(CHERISH_SRC
    ${CMAKE_SOURCE_DIR}/cherish/Data.h
    ${CMAKE_SOURCE_DIR}/cherish/Data.cpp
    ${CMAKE_SOURCE_DIR}/cherish/Utilities.h
    ${CMAKE_SOURCE_DIR}/cherish/Utilities.cpp
    ${CMAKE_SOURCE_DIR}/cherish/Settings.h
    ${CMAKE_SOURCE_DIR}/libSGControls/ViewerCommand.h
    ${CMAKE_SOURCE_DIR}/libSGControls/ViewerCommand.cpp
    ${CMAKE_SOURCE_DIR}/libSGControls/Manipulator.h
    ${CMAKE_SOURCE_DIR}/libSGControls/Manipulator.cpp
    ${CMAKE_SOURCE_DIR}/libSGControls/EventHandler.h
    ${CMAKE_SOURCE_DIR}/libSGControls/EventHandler.cpp
    )

## base GUI test to inherit, which inherits MainWindow
set(BASEGUITEST_SRC BaseGuiTest.h BaseGuiTest.cpp )

# SceneState tests with relation to bookmarks (SG and GUI) and canvas-photo widget (GUI)
set(SCENESTATE_SRC SceneStateTest.h SceneStateTest.cpp ${BASEGUITEST_SRC})
set(SCENESTATE_NAME test_SceneState)
add_executable(${SCENESTATE_NAME} ${SCENESTATE_SRC} ${CHERISH_SRC} ${IMAGE_RSC} ${BASEGUITEST_SRC})
target_link_libraries(${SCENESTATE_NAME} ${TEST_LIBRARIES})
add_test(${SCENESTATE_NAME} ${SCENESTATE_SRC} ${CHERISH_SRC} ${IMAGE_RSC} ${BASEGUITEST_SRC})

# UserScene tests
set(USERSCENE_SRC UserSceneTest.h UserSceneTest.cpp ${BASEGUITEST_SRC})
set(USERSCENE_NAME test_UserScene)
add_executable(${USERSCENE_NAME} ${USERSCENE_SRC} ${CHERISH_SRC} ${IMAGE_RSC} ${BASEGUITEST_SRC})
target_link_libraries(${USERSCENE_NAME} ${TEST_LIBRARIES})
add_test(${USERSCENE_NAME} ${USERSCENE_SRC} ${CHERISH_SRC} ${IMAGE_RSC} ${BASEGUITEST_SRC})

# MainWindow tests
set(MAINWINDOW_SRC MainWindowTest.h MainWindowTest.cpp ${BASEGUITEST_SRC})
set(MAINWINDOW_NAME test_MainWindow)
add_executable(${MAINWINDOW_NAME} ${MAINWINDOW_SRC} ${CHERISH_SRC} ${IMAGE_RSC} ${BASEGUITEST_SRC})
target_link_libraries(${MAINWINDOW_NAME} ${TEST_LIBRARIES})
add_test(${MAINWINDOW_NAME} ${MAINWINDOW_SRC} ${CHERISH_SRC} ${IMAGE_RSC} ${BASEGUITEST_SRC})
