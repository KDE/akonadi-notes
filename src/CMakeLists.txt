# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_library(KPim6AkonadiNotes)
add_library(KPim6::AkonadiNotes ALIAS KPim6AkonadiNotes)

target_sources(KPim6AkonadiNotes PRIVATE
    noteutils.cpp
    noteutils.h
    )

ecm_qt_declare_logging_category(KPim6AkonadiNotes HEADER akonadi_notes_debug.h IDENTIFIER AKONADINOTES_LOG CATEGORY_NAME log_akonadi_notes)

generate_export_header(KPim6AkonadiNotes BASE_NAME akonadi-notes)


kde_target_enable_exceptions(KPim6AkonadiNotes PUBLIC)

target_include_directories(KPim6AkonadiNotes INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim6/AkonadiNotes>")
target_include_directories(KPim6AkonadiNotes PUBLIC "$<BUILD_INTERFACE:${Akonadi-Notes_SOURCE_DIR}/src;${Akonadi-Notes_BINARY_DIR}/src;>")

target_link_libraries(KPim6AkonadiNotes
    PUBLIC
    KPim6::Mime
    PRIVATE
    Qt::Xml
    KF6::I18n
    )

set_target_properties(KPim6AkonadiNotes PROPERTIES
    VERSION ${AKONADINOTES_VERSION}
    SOVERSION ${AKONADINOTES_SOVERSION}
    EXPORT_NAME AkonadiNotes
    )

install(TARGETS
    KPim6AkonadiNotes
    EXPORT KPim6AkonadiNotesTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    )

ecm_generate_headers(AkonadiNotes_CamelCase_HEADERS
    HEADER_NAMES

    NoteUtils
    REQUIRED_HEADERS AkonadiNotes_HEADERS
    PREFIX Akonadi
    )

install(FILES
    ${AkonadiNotes_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/AkonadiNotes/Akonadi
    COMPONENT Devel
    )

install(FILES
    ${AkonadiNotes_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/akonadi-notes_export.h
    ${AkonadiNotes_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/AkonadiNotes/akonadi
    COMPONENT Devel
    )

if(BUILD_QCH)
    ecm_add_qch(
        KPim6AkonadiNotes_QCH
        NAME KPim6AkonadiNotes
        BASE_NAME KPim6AkonadiNotes
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${AkonadiNotes_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        #IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt6Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            AKONADI_NOTES_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()
