add_library(KF5AkonadiNotes)
add_library(KF5::KF5AkonadiNotes ALIAS KF5AkonadiNotes)

target_sources(KF5AkonadiNotes PRIVATE
    noteutils.cpp
    )

ecm_qt_declare_logging_category(KF5AkonadiNotes HEADER akonadi_notes_debug.h IDENTIFIER AKONADINOTES_LOG CATEGORY_NAME log_akonadi_notes)

generate_export_header(KF5AkonadiNotes BASE_NAME akonadi-notes)


kde_target_enable_exceptions(KF5AkonadiNotes PUBLIC)

target_include_directories(KF5AkonadiNotes INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/Akonadi/Notes;${KDE_INSTALL_INCLUDEDIR_KF5}/akonadi/notes>")
target_include_directories(KF5AkonadiNotes PUBLIC "$<BUILD_INTERFACE:${Akonadi-Notes_SOURCE_DIR}/src;${Akonadi-Notes_BINARY_DIR}/src;>")

target_link_libraries(KF5AkonadiNotes
    PUBLIC
    KF5::Mime
    PRIVATE
    Qt::Xml
    KF5::I18n
    )

set_target_properties(KF5AkonadiNotes PROPERTIES
    VERSION ${AKONADINOTES_VERSION}
    SOVERSION ${AKONADINOTES_SOVERSION}
    EXPORT_NAME AkonadiNotes
    )

install(TARGETS
    KF5AkonadiNotes
    EXPORT KF5AkonadiNotesTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
    )

ecm_generate_headers(AkonadiNotes_CamelCase_HEADERS
    HEADER_NAMES

    NoteUtils
    REQUIRED_HEADERS AkonadiNotes_HEADERS
    PREFIX Akonadi/Notes
    )

install(FILES
    ${AkonadiNotes_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/Akonadi/Notes
    COMPONENT Devel
    )

install(FILES
    ${AkonadiNotes_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/akonadi-notes_export.h
    ${AkonadiNotes_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/akonadi/notes
    COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME AkonadiNotes
    LIB_NAME KF5AkonadiNotes
    DEPS "KMime" FILENAME_VAR PRI_FILENAME
    )

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if (BUILD_QCH)
    ecm_add_qch(
        KF5AkonadiNotes_QCH
        NAME KF5AkonadiNotes
        BASE_NAME KF5AkonadiNotes
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${AkonadiNotes_HEADERS}
        #MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        #IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt5Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            AKONADI_NOTES_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()
