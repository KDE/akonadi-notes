cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
set(PIM_VERSION "5.17.40")

project(Akonadi-Notes VERSION ${PIM_VERSION})

# ECM setup
set(KF5_MIN_VERSION "5.80.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEGitCommitHooks)
include(ECMQtDeclareLoggingCategory)

set(QT_REQUIRED_VERSION "5.14.0")
set(AKONADI_NOTES_VERSION ${PIM_VERSION})

set(KMIMELIB_VERSION "5.17.40")
set(AKONADI_VERSION "5.17.40")

ecm_setup_version(PROJECT VARIABLE_PREFIX AKONADINOTES
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/akonadi-notes_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5AkonadiNotesConfigVersion.cmake"
    SOVERSION 5
    )

########### Find packages ###########
find_package(Qt5Xml ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

find_package(KF5I18n ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIMELIB_VERSION} CONFIG REQUIRED)
########### Targets ###########

add_definitions(-DTRANSLATION_DOMAIN=\"akonadinotes5\")

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055100)



set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
    )

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
