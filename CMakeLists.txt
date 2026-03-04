cmake_minimum_required(VERSION 4.1)
project(gletchik-studio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(gletchik-studio
        main.cpp
        resources/resources.qrc
)

target_include_directories(gletchik-studio PUBLIC include)

target_link_libraries(gletchik-studio
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

list(APPEND SOURCES "src/core/windows/MainWindow.cpp")
list(APPEND HEADERS "include/core/windows/mainwindow.h")
list(APPEND FORMS "src/ui/windows/MainWindow.ui")
