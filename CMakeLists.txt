cmake_minimum_required(VERSION 3.16)

project(TIGCC-Qt
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(
    APPEND
    CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

find_package(
	Qt6
	REQUIRED
	COMPONENTS
		Widgets
)

find_package(
    QScintilla
    REQUIRED
)

find_package(
    KF6TextEditor
    REQUIRED
)

qt_standard_project_setup()

qt_add_executable(tigcc-qt
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h

    src/project.cpp
    src/project.h

    src/projectmanager.cpp
    src/projectmanager.h

    src/projecttreewidget.cpp
    src/projecttreewidget.h

    src/buildoutputwidget.cpp
    src/buildoutputwidget.h

    src/editorbackend.cpp
    src/editorbackend.h

	src/qscintillabackend.cpp
	src/qscintillabackend.h

    src/ktexteditorbackend.cpp
    src/ktexteditorbackend.h
	
	src/appearance.cpp
	src/appearance.h
)

target_link_libraries(tigcc-qt
    PRIVATE
        Qt6::Widgets
		QScintilla::QScintilla
        KF6::TextEditor
)
