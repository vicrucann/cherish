cmake_minimum_required(VERSION 2.8.11)

include_directories(
    ${dureu3d_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/dureu3d
    ${CMAKE_SOURCE_DIR}/libGUI
    ${CMAKE_SOURCE_DIR}/libSGEntities
    ${CMAKE_SOURCE_DIR}/libSGControls
    ${CMAKE_SOURCE_DIR}/libSGUtils
)

SET( TEST_LIBRARIES
    libSGEntities
    libSGControls
    libSGUtils
    libGUI
    ${QT_QTTEST_LIBRARY}
    ${QT_LIBRARIES}
)

# for every test, produce an executable:
set(FOO_SRC foo.cpp)
set(FOO_NAME foo)
add_executable(${FOO_NAME} ${FOO_SRC})
target_link_libraries(${FOO_NAME} ${TEST_LIBRARIES})
add_test(${FOO_NAME} ${FOO_SRC})

# Root Scene tests
set(ROOT_SRC rootscenetest.cpp)
set(ROOT_NAME rootscenetest)
add_executable(${ROOT_NAME} ${ROOT_SRC})
target_link_libraries(${ROOT_NAME} ${TEST_LIBRARIES})
add_test(${ROOT_NAME} ${ROOT_SRC})
