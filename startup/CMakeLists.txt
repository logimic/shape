project (startup)

file(GLOB_RECURSE _HDRFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE _SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE _CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/configuration/*)
file(GLOB_RECURSE _CONFIG_EX ${CMAKE_CURRENT_SOURCE_DIR}/configurationExamples/*)

source_group("Header Files" FILES ${_HDRFILES})
source_group("Source Files" FILES ${_SRCFILES})
source_group("Configuration" FILES ${_CONFIG})
source_group("ConfigExamples" FILES ${_CONFIG_EX})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/launcher)

if(SHAPE_STATIC_LIBS)
    add_executable(${PROJECT_NAME} ${_HDRFILES} ${_SRCFILES} ${_CONFIG} ${_CONFIG_EX})

    target_link_libraries(${PROJECT_NAME} 
        launcher
        TraceFormatService
        TraceFileService
        CommandService
    )
else()
    add_executable(${PROJECT_NAME} ${_HDRFILES} ${_SRCFILES} ${_CONFIG} ${_CONFIG_EX})

    target_link_libraries(${PROJECT_NAME} 
        launcher
    )
endif()

if (WIN32)
    set (_CONFIG_DEST ${CMAKE_CURRENT_BINARY_DIR})
else()
    target_link_libraries(${PROJECT_NAME} 
        dl
        pthread
    )

    set (_CONFIG_DEST $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()

DeployShapeComponent(${PROJECT_NAME})

DeployShapeConfiguration(testStartup
	${CMAKE_CURRENT_SOURCE_DIR}/configuration configuration
)

DeployShapeConfigurationStartUp("${PROJECT_INSTALL_PREFIX}/bin/${PROJECT_NAME}" testStartup)

ConfigureMsvcProject("${PROJECT_INSTALL_PREFIX}/bin/${PROJECT_NAME}" testStartup)
