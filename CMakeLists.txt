cmake_minimum_required(VERSION 3.16)

project(TIGCC-Qt
    VERSION 0.1.1
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

option(ENABLE_KTEXTEDITOR "Enable KTextEditor backend")

find_package(
	qtermwidget6
	CONFIG
	REQUIRED
)

if (ENABLE_KTEXTEDITOR)
    set(USE_KTEXTEDITOR 1)
    find_package(
        KF6TextEditor
        REQUIRED
    )
endif (ENABLE_KTEXTEDITOR)

qt_standard_project_setup()

configure_file(src/config.h.in config.h)

set (SOURCES
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

    src/editorbackendfactory.cpp
    src/editorbackendfactory.h

    src/editorbackend.cpp
    src/editorbackend.h

    src/qscintillabackend.cpp
    src/qscintillabackend.h

    src/appearance.cpp
    src/appearance.h

    src/vimbackend.cpp
    src/vimbackend.h
)

if (ENABLE_KTEXTEDITOR)
    list(APPEND SOURCES
        src/ktexteditorbackend.cpp
        src/ktexteditorbackend.h
    )
endif(ENABLE_KTEXTEDITOR)

qt_add_executable(tigcc-qt
    ${SOURCES}
)

target_include_directories(tigcc-qt PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(tigcc-qt
    PRIVATE
        Qt6::Widgets
                QScintilla::QScintilla
                qtermwidget6
)


if (ENABLE_KTEXTEDITOR)
    target_link_libraries(tigcc-qt
        PRIVATE
            KF6::TextEditor
    )
endif(ENABLE_KTEXTEDITOR)
