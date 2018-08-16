
SET(GOOGLETEST_SEARCH_PATH
    "${GTEST_SOURCE_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../external/googletest/googletest")

FIND_PATH(GOOGLETEST_SOURCE_DIR
    NAMES CMakeLists.txt src/gtest_main.cc
    PATHS ${GOOGLETEST_SEARCH_PATH})


FIND_PATH(GoogleTest_INCLUDE_DIR
    NAMES gtest/gtest.h
    PATH_SUFFIXES include
    HINTS ${GOOGLETEST_SOURCE_DIR}
    PATHS ${GOOGLETEST_SEARCH_PATH})

set(GoogleTest_LIBRARIES optimized gtest debug gtestd)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GoogleTest DEFAULT_MSG
    GoogleTest_INCLUDE_DIR)
