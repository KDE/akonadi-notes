# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
include(ECMMarkAsTest)
find_package(Qt6Test ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

add_executable(notestest notestest.cpp)
add_test(NAME notestest COMMAND notestest)
ecm_mark_as_test(notestest)
target_link_libraries(notestest KPim6AkonadiNotes KPim6::Mime Qt::Test)

set(CMAKE_PREFIX_PATH ../)
