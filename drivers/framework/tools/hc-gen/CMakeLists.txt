project(hc-gen)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-Wall")

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-D OS_UNIX)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-D OS_MINGW)
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

if(CMAKE_BUILD_TYPE MATCHES Asan)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address")
endif()

aux_source_directory(src SOURCES)

add_executable(hc-gen ${SOURCES})