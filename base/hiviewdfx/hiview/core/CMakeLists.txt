set(hiview_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_platform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_config.cpp
)

set(hiview_core_heads
    ${CMAKE_CURRENT_SOURCE_DIR}/hiview_platform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_config.h
)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${hiview_core_sources}
    ${hiview_core_heads})
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})