cmake_minimum_required(VERSION 2.8.11)

include_directories(
    ${cherish_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/cherish
    ${CMAKE_SOURCE_DIR}/libGUI
    ${CMAKE_SOURCE_DIR}/libSGEntities
    ${CMAKE_SOURCE_DIR}/libSGControls
)

SET( TEST_LIBRARIES
    libSGEntities
    libSGControls
    libGUI
    ${QT_QTTEST_LIBRARY}
    ${QT_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
)

# for every test, produce an executable:
set(BASETEST_SRC BaseTest.h BaseTest.cpp)
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
