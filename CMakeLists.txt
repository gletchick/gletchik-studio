cmake_minimum_required(VERSION 3.10) # Версия 4.1 пока не существует, лучше 3.10+
project(gletchik-studio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
set(CMAKE_AUTOUIC_SEARCH_PATHS
        "resources/ui/windows"
        "resources/ui/sidebar"
)

# Группируем файлы для удобства
set(CORE_SOURCES
        # Interfaces
        sdk/steptype.h
        sdk/iprocess.h
        sdk/ibuildstep.h
        include/core/execution/isteprunner.h
        sdk/ilanguageprovider.h

        # Implementation
        src/core/processes/NativeProcess.cpp
        src/core/execution/AbstractProcessStep.cpp
        src/core/execution/StepRunner.cpp
        src/core/languages/java/JavaSteps.cpp
        include/core/execution/ibuildcontroller.h
        include/core/execution/abstractbuildcontroller.h
        src/core/execution/AbstractBuildController.cpp
        include/core/languages/java/javabuildbontroller.h
        include/core/languages/java/javalanguageprovider.h
        src/core/languages/java/JavaBuildController.cpp
        src/core/utils/JavaParser.cpp
        include/core/utils/javaparser.h
        include/core/execution/pluginmanager.h
        src/core/execution/PlaginManager.cpp
        sdk/syntaxrules.h
        include/core/utils/threadsafequeue.h
        src/core/utils/threadsafequeue.cpp
        src/core/languages/java/JavaLanguageProvider.cpp
        include/gui/terminalwidget.h
        src/gui/TerminalWidget.cpp
)

add_executable(gletchik-studio
        main.cpp
        resources/app.qrc
        ${CORE_SOURCES}
        src/gui/windows/MainWindow.cpp
        include/gui/windows/mainwindow.h
        resources/ui/windows/mainwindow.ui
        src/gui/sidebar/SideBar.cpp
        include/gui/sidebar/sidebar.h
        resources/ui/sidebar/sidebar.ui
    )

target_include_directories(gletchik-studio PUBLIC include)

target_link_libraries(gletchik-studio
        PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        dl
)

set_target_properties(gletchik-studio PROPERTIES ENABLE_EXPORTS ON)
target_link_options(gletchik-studio PRIVATE "-rdynamic")