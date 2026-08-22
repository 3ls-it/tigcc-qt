cmake_minimum_required(VERSION 3.16)

project(TIGCC-Qt
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(tigcc-qt
    src/main.cpp
	src/mainwindow.cpp
    src/mainwindow.h
	src/projecttreewidget.cpp
    src/projecttreewidget.h
    src/editorwidget.cpp
    src/editorwidget.h
    src/buildoutputwidget.cpp
    src/buildoutputwidget.h
)

target_link_libraries(tigcc-qt
    PRIVATE
        Qt6::Widgets
)
