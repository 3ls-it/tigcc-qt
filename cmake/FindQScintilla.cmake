include(FindPackageHandleStandardArgs)

set(
    QScintilla_INCLUDE_SEARCH_PATHS
    /usr/local/include
    /usr/include
	${PREFIX}/include
)

set(
    QScintilla_INCLUDE_SUFFIXES
    qt6
)

if(CMAKE_LIBRARY_ARCHITECTURE)
    list(
        APPEND
        QScintilla_INCLUDE_SUFFIXES
        "${CMAKE_LIBRARY_ARCHITECTURE}/qt6"
    )
endif()

find_path(
    QScintilla_INCLUDE_DIR
    NAMES
        Qsci/qsciscintilla.h
    PATHS
        ${QScintilla_INCLUDE_SEARCH_PATHS}
    PATH_SUFFIXES
        ${QScintilla_INCLUDE_SUFFIXES}
)

find_library(
    QScintilla_LIBRARY
    NAMES
        qscintilla2_qt6
        libqscintilla2_qt6
    PATHS
        /usr/local
        /usr
        /data/data/com.termux/files/usr
    PATH_SUFFIXES
        lib
        lib64
        lib/qt6
)

find_package_handle_standard_args(
    QScintilla
    REQUIRED_VARS
        QScintilla_INCLUDE_DIR
        QScintilla_LIBRARY
)

if(QScintilla_FOUND)
    if(NOT TARGET QScintilla::QScintilla)
        add_library(
            QScintilla::QScintilla
            UNKNOWN IMPORTED
        )

        set_target_properties(
            QScintilla::QScintilla
            PROPERTIES
                IMPORTED_LOCATION
                    "${QScintilla_LIBRARY}"

                INTERFACE_INCLUDE_DIRECTORIES
                    "${QScintilla_INCLUDE_DIR}"

                INTERFACE_LINK_LIBRARIES
                    Qt6::Widgets
        )
    endif()
endif()

mark_as_advanced(
    QScintilla_INCLUDE_DIR
    QScintilla_LIBRARY
)
