file(GLOB utility_headfiles include/*.h)
file(GLOB utility_extra_headfiles include/extra/*.h)

set(utility_sources
     ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_module.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/time_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/file_util.cpp
)

if(WIN32)
    file(GLOB utility_platform_sources platform/windows/*.cpp)
elseif(UNIX)
    set(utility_platform_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/linux/time_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/linux/thread_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/linux/file_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/ohos/common_utils.cpp)
endif()

target_sources(${PROJECT_NAME}
    PRIVATE
    ${utility_sources}
    ${utility_platform_sources}
    ${utility_headfiles}
    ${utility_extra_headfiles})
target_include_directories(${PROJECT_NAME} PUBLIC
        include
        include/extra)