# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_library(KPim${KF_MAJOR_VERSION}AkonadiNotes)
add_library(KPim${KF_MAJOR_VERSION}::AkonadiNotes ALIAS KPim${KF_MAJOR_VERSION}AkonadiNotes)
add_library(KF5::AkonadiNotes ALIAS KPim${KF_MAJOR_VERSION}AkonadiNotes)

target_sources(KPim${KF_MAJOR_VERSION}AkonadiNotes PRIVATE
    noteutils.cpp
    noteutils.h
    )

ecm_qt_declare_logging_category(KPim${KF_MAJOR_VERSION}AkonadiNotes HEADER akonadi_notes_debug.h IDENTIFIER AKONADINOTES_LOG CATEGORY_NAME log_akonadi_notes)

generate_export_header(KPim${KF_MAJOR_VERSION}AkonadiNotes BASE_NAME akonadi-notes)


kde_target_enable_exceptions(KPim${KF_MAJOR_VERSION}AkonadiNotes PUBLIC)

target_include_directories(KPim${KF_MAJOR_VERSION}AkonadiNotes INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/AkonadiNotes>")
target_include_directories(KPim${KF_MAJOR_VERSION}AkonadiNotes PUBLIC "$<BUILD_INTERFACE:${Akonadi-Notes_SOURCE_DIR}/src;${Akonadi-Notes_BINARY_DIR}/src;>")

target_link_libraries(KPim${KF_MAJOR_VERSION}AkonadiNotes
    PUBLIC
    KPim${KF_MAJOR_VERSION}::Mime
    PRIVATE
    Qt::Xml
    KF${KF_MAJOR_VERSION}::I18n
    )

set_target_properties(KPim${KF_MAJOR_VERSION}AkonadiNotes PROPERTIES
    VERSION ${AKONADINOTES_VERSION}
    SOVERSION ${AKONADINOTES_SOVERSION}
    EXPORT_NAME AkonadiNotes
    )

install(TARGETS
    KPim${KF_MAJOR_VERSION}AkonadiNotes
    EXPORT KPim${KF_MAJOR_VERSION}AkonadiNotesTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS}
    )

ecm_generate_headers(AkonadiNotes_CamelCase_HEADERS
    HEADER_NAMES

    NoteUtils
    REQUIRED_HEADERS AkonadiNotes_HEADERS
    PREFIX Akonadi
    )

install(FILES
    ${AkonadiNotes_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/AkonadiNotes/Akonadi
    COMPONENT Devel
    )

install(FILES
    ${AkonadiNotes_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/akonadi-notes_export.h
    ${AkonadiNotes_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/AkonadiNotes/akonadi
    COMPONENT Devel
    )

ecm_generate_pri_file(BASE_NAME AkonadiNotes
    LIB_NAME KF${KF_MAJOR_VERSION}AkonadiNotes
    DEPS "KMime" FILENAME_VAR PRI_FILENAME
    )

install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if (BUILD_QCH)
    ecm_add_qch(
        KPim${KF_MAJOR_VERSION}AkonadiNotes_QCH
        NAME KPim${KF_MAJOR_VERSION}AkonadiNotes
        BASE_NAME KPim${KF_MAJOR_VERSION}AkonadiNotes
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${AkonadiNotes_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        #IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt${QT_MAJOR_VERSION}Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            AKONADI_NOTES_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()
