# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "6.1.2")

project(Akonadi-Notes VERSION ${PIM_VERSION})

# ECM setup
set(KF_MIN_VERSION "6.0.0")
set(QT_REQUIRED_VERSION "6.6.0")
find_package(ECM ${KF_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(GenerateExportHeader)
include(ECMGenerateHeaders)

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMFeatureSummary)
include(ECMAddQch)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

set(AKONADI_NOTES_VERSION ${PIM_VERSION})

set(KMIMELIB_VERSION "6.1.2")
set(AKONADI_VERSION "6.1.2")

ecm_setup_version(PROJECT VARIABLE_PREFIX AKONADINOTES
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/akonadi-notes_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6AkonadiNotesConfigVersion.cmake"
    SOVERSION 6
    )

########### Find packages ###########
find_package(Qt6Xml ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

find_package(KF6I18n ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KPim6Mime ${KMIMELIB_VERSION} CONFIG REQUIRED)
########### Targets ###########

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()

add_definitions(-DTRANSLATION_DOMAIN=\"akonadinotes6\")

add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)
ecm_set_disabled_deprecation_versions(QT 6.7.0 KF 6.1.0)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6AkonadiNotes")

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
########### CMake Config Files ###########

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/akonadi-notes_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/AkonadiNotes COMPONENT Devel
    )

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KPim6AkonadiNotes_QCH
        FILE KPim6AkonadiNotesQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim6AkonadiNotesQchTargets.cmake\")")
endif()

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimAkonadiNotesConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6AkonadiNotesConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6AkonadiNotesConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6AkonadiNotesConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim6AkonadiNotesTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6AkonadiNotesTargets.cmake NAMESPACE KPim6::)



ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
