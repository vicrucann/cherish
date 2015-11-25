project(test_dureu3d)
cmake_minimum_required(VERSION 2.8.11)

enable_testing()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Test REQUIRED)

set(SOURCES
	foo.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
add_test(${PROJECT_NAME} foo)

target_link_libraries( ${PROJECT_NAME}
	Qt5::Test
)