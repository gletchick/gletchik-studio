cmake_minimum_required(VERSION 3.10) # Версия 4.1 пока не существует, лучше 3.10+
project(gletchik-studio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS src/ui/windows)

# Группируем файлы для удобства
set(CORE_SOURCES
        # Interfaces
        include/core/steptype.h
        include/core/processes/iprocess.h
        include/core/execution/ibuildstep.h
        include/core/execution/isteprunner.h
        include/core/languages/ilanguageprovider.h

        # Implementation
        src/core/processes/nativeprocess.cpp
        src/core/execution/abstractprocessstep.cpp
        src/core/execution/steprunner.cpp
        src/core/languages/java/javasteps.cpp
        src/core/languages/java/javalanguageprovider.cpp
        include/core/execution/ibuildcontroller.h
        include/core/execution/abstractbuildcontroller.h
        src/core/execution/AbstractBuildController.cpp
        include/core/languages/java/javabuildbontroller.h
        src/core/languages/java/JavaBuildController.cpp
        src/ui/JavaParser.cpp
        include/core/utils/javaparser.h
)

add_executable(gletchik-studio
        main.cpp
        resources/resources.qrc
        ${CORE_SOURCES}
)

# Это позволит писать #include "core/..." без полных путей
target_include_directories(gletchik-studio PUBLIC include)

target_link_libraries(gletchik-studio
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)