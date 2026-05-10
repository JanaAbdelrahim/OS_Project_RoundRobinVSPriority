cmake_minimum_required(VERSION 3.15)
project(OperatingSystemProject-GUI)

set(CMAKE_CXX_STANDARD 20)

# SFML bundled inside project — no install needed
set(SFML_DIR "${CMAKE_SOURCE_DIR}/SFML/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

include_directories(include)

add_executable(OperatingSystemProject-GUI
        include/algorithms/PriorityScheduler.h
        include/algorithms/RoundRobin.h
        include/core/GanttEntry.h
        include/core/Process.h
        include/core/SchedulerBase.h
        include/core/SimulationResult.h
        include/utils/Validator.h
        src/algorithms/PriorityScheduler.cpp
        src/algorithms/RoundRobin.cpp
        src/core/Process.cpp
        src/core/SimulationResult.cpp
        src/core/SchedulerBase.cpp
        src/utils/Validator.cpp
        src/main.cpp
        include/gui/MainWindow.h
        src/gui/MainWindow.cpp
        src/gui/UIComponents.cpp
)

target_link_libraries(OperatingSystemProject-GUI
        sfml-graphics
        sfml-window
        sfml-system
)

# Copy SFML DLLs next to .exe after every build
add_custom_command(TARGET OperatingSystemProject-GUI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/SFML/bin"
        "$<TARGET_FILE_DIR:OperatingSystemProject-GUI>"
        COMMENT "Copying SFML DLLs..."
)