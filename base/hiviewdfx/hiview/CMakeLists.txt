cmake_minimum_required(VERSION 3.5)

# Ensure that CMAKE_BUILD_TYPE has a value specified for single configuration generators.
if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Coverage.")
endif()

project(hiview_base
     LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#---------build option-----------
option(BUILD_SHARED_LIBS "Build hiview as a shared library." OFF)
option(BUILD_WITH_HILOG "Build with Hilog" OFF)

# Adhere to GNU filesystem layout conventions
include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Archive output dir.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Library output dir.")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin" CACHE PATH "PDB MSVC debug symbol output dir.")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Executable/dll output dir.")

if(WIN32)
elseif(UNIX)
     add_compile_options(
            -Wall
            -Wno-format
            -Wdate-time
            -Wextra 
            -Wno-sign-compare
            -Wno-unused-parameter
            -Wno-unused-result
            -Wno-missing-field-initializers
            -fno-common
            -fno-strict-aliasing
            -fvisibility=hidden
            -Os -fPIC)
endif()

add_library(${PROJECT_NAME} STATIC)
# in windows and linux default build with no hilog
if(BUILD_WITH_HILOG)

else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE __WITH_NO_HILOG__)
endif(BUILD_WITH_HILOG)

add_subdirectory(adapter)
add_subdirectory(base)
add_subdirectory(core)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)