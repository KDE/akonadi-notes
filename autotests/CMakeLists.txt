# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
include(ECMMarkAsTest)
find_package(Qt${QT_MAJOR_VERSION}Test ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

add_executable(notestest notestest.cpp)
add_test(NAME notestest COMMAND notestest)
ecm_mark_as_test(notestest)
target_link_libraries(notestest KPim${KF_MAJOR_VERSION}AkonadiNotes KPim${KF_MAJOR_VERSION}::Mime Qt::Test)

set(CMAKE_PREFIX_PATH ../)
