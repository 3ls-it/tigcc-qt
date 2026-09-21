cmake_minimum_required(VERSION 3.16)

project(TIGCC-Qt
    VERSION 0.2.4
    LANGUAGES CXX
)

set(
    TIGCC_QT_RELEASE_LABEL
    "0.2.4-alpha"
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

option(ENABLE_KTEXTEDITOR "Enable KTextEditor backend" OFF)

find_package(
	qtermwidget6
	CONFIG
	REQUIRED
)

if (ENABLE_KTEXTEDITOR)
    find_package(
        KF6TextEditor
        REQUIRED
    )
	list(
			APPEND
			SOURCES
			src/ktexteditorbackend.cpp
			src/ktexteditorbackend.h
	)
endif (ENABLE_KTEXTEDITOR)

qt_standard_project_setup()

configure_file(src/config.h.in config.h)

set (SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h

    src/appearance.cpp
    src/appearance.h

    src/configuration.cpp
    src/configuration.h

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

	src/qscintillatheme.cpp
	src/qscintillatheme.h

    src/vimbackend.cpp
    src/vimbackend.h

	src/completionentry.h
	src/completiondatabackend.cpp
	src/completiondatabackend.h
	src/completiondatalocator.cpp
	src/completiondatalocator.h
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

if(ENABLE_KTEXTEDITOR)
	target_compile_definitions(
		tigcc-qt
		PRIVATE
			USE_KTEXTEDITOR
	)
endif()

target_include_directories(tigcc-qt PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

include(
	GNUInstallDirs
)

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


install(
	FILES
		data/tigcc-completions.json
	DESTINATION
		${CMAKE_INSTALL_DATADIR}/tigcc-qt
)
