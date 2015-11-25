project(dureu3d)
cmake_minimum_required(VERSION 2.8.11)

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

IF( CMAKE_COMPILER_IS_GNUCXX )
    # Add additional GCC options.
    ADD_DEFINITIONS(
        -Wall -Wundef -Wcast-align -Wchar-subscripts -Wpointer-arith
        -Wwrite-strings -Wpacked -Wformat-security
        -Wmissing-format-attribute -Wold-style-cast
    )
    ADD_DEFINITIONS( -fvisibility=hidden )
ELSEIF( CMAKE_CXX_COMPILER MATCHES "clang" )
    ADD_DEFINITIONS( -Wall -Wextra -Wno-unused-parameter )
    ADD_DEFINITIONS( -fvisibility=hidden )
ENDIF()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

#SET( CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${CMAKE_CXX_FLAGS}" )

set(QT_LIBRARIES
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5OpenGL_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
)
set(QT_QTTEST_LIBRARY ${Qt5Test_LIBRARIES})

enable_testing()

add_subdirectory(dureu3d)
add_subdirectory(libGUI)
add_subdirectory(libSGEntities)
add_subdirectory(libSGControls)
add_subdirectory(libSGUtils)
add_subdirectory(tests)
