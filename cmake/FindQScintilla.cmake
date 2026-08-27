include(FindPackageHandleStandardArgs)

find_path(
    QScintilla_INCLUDE_DIR
    NAMES
        Qsci/qsciscintilla.h
    PATHS
        /usr/local
        /usr
        /data/data/com.termux/files/usr
    PATH_SUFFIXES
        include/qt6
        include
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
