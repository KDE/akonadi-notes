# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "5.21.2")

project(Akonadi-Notes VERSION ${PIM_VERSION})

# ECM setup
set(KF5_MIN_VERSION "5.95.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMAddQch)


option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

set(QT_REQUIRED_VERSION "5.15.2")
set(AKONADI_NOTES_VERSION ${PIM_VERSION})

set(KMIMELIB_VERSION "5.21.2")
set(AKONADI_VERSION "5.21.2")

ecm_setup_version(PROJECT VARIABLE_PREFIX AKONADINOTES
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/akonadi-notes_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5AkonadiNotesConfigVersion.cmake"
    SOVERSION 5
    )

########### Find packages ###########
find_package(Qt${QT_MAJOR_VERSION}Xml ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

find_package(KF5I18n ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIMELIB_VERSION} CONFIG REQUIRED)
########### Targets ###########

add_definitions(-DTRANSLATION_DOMAIN=\"akonadinotes5\")

ecm_set_disabled_deprecation_versions(QT 5.15.2 KF 5.96.0)




add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5AkonadiNotes")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5AkonadiNotesConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5AkonadiNotesConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5AkonadiNotesConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5AkonadiNotesConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KF5AkonadiNotesTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5AkonadiNotesTargets.cmake NAMESPACE KF5::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/akonadi-notes_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/AkonadiNotes COMPONENT Devel
    )

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5AkonadiNotes_QCH
        FILE KF5AkonadiNotesQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5AkonadiNotesQchTargets.cmake\")")
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
