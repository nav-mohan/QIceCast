cmake_minimum_required(VERSION 3.10)

project(test_mainwindow VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
enable_testing(true)

get_filename_component(PROJECT_BASE_DIR ../../ ABSOLUTE)
set(SOURCE_DIR ${PROJECT_BASE_DIR}/UI)
set(LAYOUT_DIR ${PROJECT_BASE_DIR}/Layout)
include_directories(${SOURCE_DIR}) # points to mainwindow.h/.cpp
include_directories(${PROJECT_BASE_DIR}) # points to constants.h

set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt@5/5.15.8_1")
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Multimedia Test)

set(
    CPP_SOURCE_FILES 
	main.cpp 
    mainwindow.tests.h
    ${SOURCE_DIR}/mainwindow.cpp
    ${SOURCE_DIR}/mountpointsettingdialog.cpp
	)
qt_wrap_ui(
    UI_FILES 
	${LAYOUT_DIR}/mainwindow.ui 
	${LAYOUT_DIR}/mountpointsettingdialog.ui 
	)
add_executable(
    ${PROJECT_NAME} 
    ${CPP_SOURCE_FILES} 
    ${UI_FILES}
    )

add_test((NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}))
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia Qt5::Test)
