project(GTestStaticRunner)

find_package(GoogleTest REQUIRED)

message(STATUS "GoogleTest_INCLUDE_DIR: ${GoogleTest_INCLUDE_DIR}")
message(STATUS "GoogleTest_LIBRARIES: ${GoogleTest_LIBRARIES}")

file(GLOB_RECURSE _HDRFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h  ${COMPONENT_HXX})
file(GLOB_RECURSE _SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

source_group("Header Files" FILES ${_HDRFILES})
source_group("Source Files" FILES ${_SRCFILES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${GoogleTest_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC ${_HDRFILES} ${_SRCFILES})
