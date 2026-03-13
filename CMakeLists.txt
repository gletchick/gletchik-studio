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
        include/core/StepType.h
        include/core/processes/IProcess.h
        include/core/execution/IBuildStep.h
        include/core/execution/IStepRunner.h
        include/core/languages/ILanguageProvider.h

        # Implementation
        src/core/processes/NativeProcess.cpp
        src/core/execution/AbstractProcessStep.cpp
        src/core/execution/StepRunner.cpp
        src/core/languages/java/JavaSteps.cpp
        src/core/languages/java/JavaLanguageProvider.cpp
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