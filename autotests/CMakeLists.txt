include(ECMMarkAsTest)
find_package(Qt5Test ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

add_executable(notestest notestest.cpp)
add_test(NAME notestest COMMAND notestest)
ecm_mark_as_test(notestest)
target_link_libraries(notestest KF5AkonadiNotes KF5::Mime Qt::Test)

set(CMAKE_PREFIX_PATH ../)
