cmake_minimum_required(VERSION 3.10)
project(QIceCast LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON) # Commented this out because I want to place my .ui files in a different folder

set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt@5/5.15.8_2")
set(QT_REQUIRED_PACKAGES Core Gui Widgets Multimedia)
find_package(Qt5 REQUIRED COMPONENTS ${QT_REQUIRED_PACKAGES})

set(CMAKE_INCLUDE_CURRENT_DIR ON) # #include "constants.h"
set(
    CPP_SOURCE_FILES 
	main.cpp 
	UI/mainwindow.cpp 
	UI/mountpointsettingdialog.cpp 
	UI/mountpointwidget.cpp 
	UI/audiolevel.cpp 
	InputDevice/inputbuffer.cpp 
	InputDevice/circularbuffer.cpp 
	InputDevice/inputdevice.cpp 
    Encoders/encoder.cpp 
    Encoders/mpegencoder.cpp 
    Encoders/aacencoder.cpp 
    Encoders/encoderfactory.cpp 
    Socket/socket.cpp 
	)
qt_wrap_ui( 
    UI_FILES 
    Layout/mainwindow.ui 
    Layout/mountpointsettingdialog.ui 
    Layout/mountpointwidget.ui 
	)
add_executable(
    ${PROJECT_NAME}
    ${CPP_SOURCE_FILES}
    ${UI_FILES}
    )

set(APP_INCLUDE_DIR Layout UI InputDevice Encoders Socket)
set(QT_LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia)
set(LAME_LINK_LIBRARY /opt/homebrew/Cellar/lame/3.100/lib/libmp3lame.a)
set(LAME_INCLUDE_DIR /opt/homebrew/Cellar/lame/3.100/include)
set(AAC_LINK_LIBRARY /opt/homebrew/Cellar/fdk-aac/2.0.2/lib/libfdk-aac.a)
set(AAC_INCLUDE_DIR /opt/homebrew/Cellar/fdk-aac/2.0.2/include)
target_include_directories(
    ${PROJECT_NAME} PUBLIC 
    ${LAME_INCLUDE_DIR} 
    ${AAC_INCLUDE_DIR} 
    ${APP_INCLUDE_DIR}
    )
target_link_libraries(
    ${PROJECT_NAME} PUBLIC 
    ${QT_LINK_LIBRARIES} 
    ${LAME_LINK_LIBRARY} 
    ${AAC_LINK_LIBRARY}
    )
