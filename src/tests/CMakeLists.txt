cmake_minimum_required(VERSION 2.8.11)

# could be replaced by ${dureu3d_SOURCE_DIR}?
include_directories( 
	${CMAKE_SOURCE_DIR}/dureu3d
	${CMAKE_SOURCE_DIR}/libGUI
	${CMAKE_SOURCE_DIR}/libSGEntities
    	${CMAKE_SOURCE_DIR}/libSGControls
    	${CMAKE_SOURCE_DIR}/libSGUtils 
)
enable_testing()
set(CMAKE_AUTOMOC ON)
find_package(Qt5Test REQUIRED)

# for every test, produce an executable:
set(FOO_SRC foo.cpp)
set(FOO_NAME foo)
add_executable(${FOO_NAME} ${FOO_SRC})
target_link_libraries(${FOO_NAME} Qt5::Test)
add_test(${FOO_NAME} ${FOO_SRC}) # or could be (foo FOO_SRC)

# Root Scene tests
set(ROOT_SRC test_rootscene.cpp)
set(ROOT_NAME test_rootscene)
add_executable(${ROOT_NAME} ${ROOT_SRC})
target_link_libraries(${ROOT_NAME} Qt5::Test)
add_test(${ROOT_NAME} ${ROOT_SRC})
