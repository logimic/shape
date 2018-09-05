
SET(GTEST_SEARCH_PATH
    "${GTEST_SOURCE_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../external/googletest/googletest")

SET(GMOCK_SEARCH_PATH
    "${GMOCK_SOURCE_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../external/googletest/googlemock")

FIND_PATH(GTEST_SOURCE_DIR
    NAMES CMakeLists.txt src/gtest_all.cc
    PATHS ${GTEST_SEARCH_PATH})

FIND_PATH(GMOCK_SOURCE_DIR
    NAMES CMakeLists.txt src/gmock_all.cc
    PATHS ${GMOCK_SEARCH_PATH})

FIND_PATH(GTEST_INCLUDE_DIR
    NAMES gtest/gtest.h
    PATH_SUFFIXES include
    HINTS ${GTEST_SOURCE_DIR}
    PATHS ${GTEST_SEARCH_PATH})

FIND_PATH(GMOCK_INCLUDE_DIR
    NAMES gmock/gmock.h
    PATH_SUFFIXES include
    HINTS ${GMOCK_SOURCE_DIR}
    PATHS ${GMOCK_SEARCH_PATH})

set(GoogleTest_INCLUDE_DIR
    ${GTEST_INCLUDE_DIR}
    ${GMOCK_INCLUDE_DIR})

set(GoogleTest_LIBRARIES optimized gtest optimized gmock debug gtestd debug gmockd)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GoogleTest DEFAULT_MSG
    GoogleTest_INCLUDE_DIR)
