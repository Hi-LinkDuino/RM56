# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Description: The configuration of toolchain file supplied to cmake, which specifies
#              locations for compilers and toolchain utilities, and other target
#              platform and compiler related information.

cmake_minimum_required(VERSION 3.6.0)
set(CMAKE_SYSTEM_VERSION 1)

if(DEFINED OHOS_SDK_NATIVE_TOOLCHAIN_DEFINED)
  return()
endif()
set(OHOS_SDK_NATIVE_TOOLCHAIN_DEFINED true)

# Set OHOS_SDK_NATIVE
get_filename_component(OHOS_SDK_NATIVE "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
file(TO_CMAKE_PATH "${OHOS_SDK_NATIVE}" OHOS_SDK_NATIVE)

# Sdk native version
file(STRINGS "${OHOS_SDK_NATIVE}/oh-uni-package.json" NATIVE_VER REGEX "\"version\":.*")
string(REGEX REPLACE "\"version\":(.*)$" "\\1" SDK_NATIVE_VERSION "${NATIVE_VER}")
string(STRIP "${SDK_NATIVE_VERSION}" SDK_NATIVE_VERSION)

# Common default settings
set(OHOS TRUE)
set(CMAKE_SYSTEM_NAME OHOS)

if(NOT DEFINED OHOS_PLATFORM_LEVEL)
  set(OHOS_PLATFORM_LEVEL 1)
endif()

if(NOT DEFINED OHOS_TOOLCHAIN)
  set(OHOS_TOOLCHAIN clang)
endif()

if(NOT DEFINED OHOS_STL)
  set(OHOS_STL c++_shared)
endif()

if(NOT DEFINED OHOS_PIE)
  set(OHOS_PIE TRUE)
endif()

if(NOT DEFINED OHOS_ARM_NEON)
  set(OHOS_ARM_NEON thumb)
endif()

# set the ABI
if(NOT DEFINED OHOS_ARCH)
  set(OHOS_ARCH arm64-v8a)
endif()

# set the undefined symbols
if(DEFINED OHOS_NO_UNDEFINED)
  if(NOT DEFINED OHOS_ALLOW_UNDEFINED_SYMBOLS)
    set(OHOS_ALLOW_UNDEFINED_SYMBOLS "${OHOS_NO_UNDEFINED}")
  endif()
endif()

# set the ccache
if(DEFINED SDK_NATIVE_CCACHE AND NOT DEFINED OHOS_CCACHE)
  set(OHOS_CCACHE "${SDK_NATIVE_CCACHE}")
endif()

# set the sdk native platform
include(${CMAKE_CURRENT_LIST_DIR}/sdk_native_platforms.cmake)
if(NOT DEFINED OHOS_SDK_NATIVE_PLATFORM)
  set(OHOS_SDK_NATIVE_PLATFORM "ohos-${SDK_NATIVE_MIN_PLATFORM_LEVEL}")
endif()

# set the sdk native platform level
string(REPLACE "ohos-" "" OHOS_SDK_NATIVE_PLATFORM_LEVEL ${OHOS_SDK_NATIVE_PLATFORM})

# set find executable programs on the host system path
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
list(APPEND CMAKE_FIND_ROOT_PATH "${OHOS_SDK_NATIVE}")

# set the arch abi
set(CMAKE_OHOS_ARCH_ABI ${OHOS_ARCH})

# set arch diff property ...
if(OHOS_ARCH STREQUAL arm64-v8a)
  set(OHOS_TOOLCHAIN_NAME aarch64-linux-ohos)
  set(OHOS_LLVM ${OHOS_TOOLCHAIN_NAME})
  set(CMAKE_SYSTEM_PROCESSOR aarch64)
elseif(OHOS_ARCH STREQUAL armeabi-v7a)
  set(OHOS_TOOLCHAIN_NAME arm-linux-ohos)
  set(OHOS_LLVM ${OHOS_TOOLCHAIN_NAME})
  set(CMAKE_SYSTEM_PROCESSOR arm)
elseif(OHOS_ARCH STREQUAL x86_64)
  set(OHOS_TOOLCHAIN_NAME x86_64-linux-ohos)
  set(OHOS_LLVM ${OHOS_TOOLCHAIN_NAME})
  set(CMAKE_SYSTEM_PROCESSOR x86_64)
else()
  message(FATAL_ERROR "unrecognized ${OHOS_ARCH}")
endif()

set(CMAKE_C_COMPILER_TARGET   ${OHOS_LLVM})
set(CMAKE_CXX_COMPILER_TARGET ${OHOS_LLVM})
set(CMAKE_ASM_COMPILER_TARGET ${OHOS_LLVM})

# Export configurable variables for the try_compile() command.
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
  OHOS_TOOLCHAIN
  OHOS_ARCH
  OHOS_PLATFORM)

# Set the common c flags
set(OHOS_C_COMPILER_FLAGS)
list(APPEND OHOS_C_COMPILER_FLAGS
  -g
  -fdata-sections
  -ffunction-sections
  -funwind-tables
  -fstack-protector-strong
  -no-canonical-prefixes
  -fno-addrsig
  -Wa,--noexecstack)
if(OHOS_DISABLE_FORMAT_STRING_CHECKS)
  list(APPEND OHOS_C_COMPILER_FLAGS -Wno-error=format-security)
else()
  list(APPEND OHOS_C_COMPILER_FLAGS -Wformat -Werror=format-security)
endif()
if (OHOS_ARCH STREQUAL armeabi-v7a)
    list(APPEND OHOS_C_COMPILER_FLAGS -march=armv7a)
endif()
string(REPLACE ";" " " OHOS_C_COMPILER_FLAGS "${OHOS_C_COMPILER_FLAGS}")

# set the common c++ flags
set(OHOS_CXX_COMPILER_FLAGS)

# set the common asm flags
set(OHOS_ASM_COMPILER_FLAGS "${OHOS_C_COMPILER_FLAGS}")

# set the debug variant flags
set(OHOS_DEBUG_COMPILER_FLAGS)
list(APPEND OHOS_DEBUG_COMPILER_FLAGS -O0 -fno-limit-debug-info)
string(REPLACE ";" " " OHOS_DEBUG_COMPILER_FLAGS   "${OHOS_DEBUG_COMPILER_FLAGS}")

# set the release variant flags
set(OHOS_RELEASE_COMPILER_FLAGS)
list(APPEND OHOS_RELEASE_COMPILER_FLAGS -O2)
list(APPEND OHOS_RELEASE_COMPILER_FLAGS -DNDEBUG)
string(REPLACE ";" " " OHOS_RELEASE_COMPILER_FLAGS "${OHOS_RELEASE_COMPILER_FLAGS}")

# set the common link flags
set(OHOS_COMMON_LINKER_FLAGS)
list(APPEND OHOS_COMMON_LINKER_FLAGS --rtlib=compiler-rt)
list(APPEND OHOS_COMMON_LINKER_FLAGS -fuse-ld=lld)

if(OHOS_STL STREQUAL c++_static)
  list(APPEND OHOS_COMMON_LINKER_FLAGS "-static-libstdc++")
elseif(OHOS_STL STREQUAL none)
  list(APPEND OHOS_CXX_COMPILER_FLAGS "-nostdinc++")
  list(APPEND OHOS_COMMON_LINKER_FLAGS "-nostdlib++")
elseif(OHOS_STL STREQUAL c++_shared)
else()
  message(FATAL_ERROR "Unsupported STL configuration: ${OHOS_STL}.")
endif()

list(APPEND OHOS_COMMON_LINKER_FLAGS
  -Wl,--build-id=sha1
  -Wl,--warn-shared-textrel
  -Wl,--fatal-warnings
  -lunwind)
if(NOT OHOS_ALLOW_UNDEFINED_SYMBOLS)
  list(APPEND OHOS_COMMON_LINKER_FLAGS -Wl,--no-undefined)
endif()
list(APPEND OHOS_COMMON_LINKER_FLAGS -Qunused-arguments -Wl,-z,noexecstack)
string(REPLACE ";" " " OHOS_COMMON_LINKER_FLAGS "${OHOS_COMMON_LINKER_FLAGS}")

# set the executable link flags
set(OHOS_EXE_LINKER_FLAGS)
list(APPEND OHOS_EXE_LINKER_FLAGS -Wl,--gc-sections)
string(REPLACE ";" " " OHOS_EXE_LINKER_FLAGS "${OHOS_EXE_LINKER_FLAGS}")

# set the other flags
set(CMAKE_C_STANDARD_LIBRARIES_INIT "-lm")
set(CMAKE_CXX_STANDARD_LIBRARIES_INIT "-lm")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# set the cmake global cflags
set(CMAKE_C_FLAGS "" CACHE STRING "Flags for all build types.")
set(CMAKE_C_FLAGS "${OHOS_C_COMPILER_FLAGS} ${CMAKE_C_FLAGS} -D__MUSL__")

set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "Flags for debug variant builds.")
set(CMAKE_C_FLAGS_DEBUG "${OHOS_DEBUG_COMPILER_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")

set(CMAKE_C_FLAGS_RELEASE "" CACHE STRING "Flags for release variant builds.")
set(CMAKE_C_FLAGS_RELEASE "${OHOS_RELEASE_COMPILER_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")

# set the cmake global cppflags
set(CMAKE_CXX_FLAGS "" CACHE STRING "Flags for all build types.")
set(CMAKE_CXX_FLAGS "${OHOS_C_COMPILER_FLAGS} ${OHOS_CXX_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS} -D__MUSL__")

set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "Flags for debug variant builds.")
set(CMAKE_CXX_FLAGS_DEBUG "${OHOS_DEBUG_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")

set(CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "Flags for release variant builds.")
set(CMAKE_CXX_FLAGS_RELEASE "${OHOS_RELEASE_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")

# set the cmake global asmflags
set(CMAKE_ASM_FLAGS "" CACHE STRING "Flags for all build types.")
set(CMAKE_ASM_FLAGS "${OHOS_ASM_COMPILER_FLAGS} ${CMAKE_ASM_FLAGS} -D__MUSL__")

set(CMAKE_ASM_FLAGS_DEBUG "" CACHE STRING "Flags for debug variant builds.")
set(CMAKE_ASM_FLAGS_DEBUG "${OHOS_DEBUG_COMPILER_FLAGS} ${CMAKE_ASM_FLAGS_DEBUG}")

set(CMAKE_ASM_FLAGS_RELEASE "" CACHE STRING "Flags for release variant builds.")
set(CMAKE_ASM_FLAGS_RELEASE "${OHOS_RELEASE_COMPILER_FLAGS} ${CMAKE_ASM_FLAGS_RELEASE}")

# set the link flags
set(CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING "Linker flags to be used to create shared libraries.")
set(CMAKE_SHARED_LINKER_FLAGS "${OHOS_COMMON_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")

set(CMAKE_MODULE_LINKER_FLAGS "" CACHE STRING "Linker flags to be used to create modules.")
set(CMAKE_MODULE_LINKER_FLAGS "${OHOS_COMMON_LINKER_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "Linker flags to be used to create executables.")
set(CMAKE_EXE_LINKER_FLAGS "${OHOS_COMMON_LINKER_FLAGS} ${OHOS_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")

# set the executable suffix
set(HOST_SYSTEM_EXE_SUFFIX)
if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
  set(HOST_SYSTEM_EXE_SUFFIX .exe)
endif()

# set the toolchain config.
set(TOOLCHAIN_ROOT_PATH "${OHOS_SDK_NATIVE}/llvm")
set(TOOLCHAIN_BIN_PATH  "${OHOS_SDK_NATIVE}/llvm/bin")

set(CMAKE_SYSROOT "${OHOS_SDK_NATIVE}/sysroot")
set(CMAKE_LIBRARY_ARCHITECTURE "${OHOS_TOOLCHAIN_NAME}")
list(APPEND CMAKE_SYSTEM_LIBRARY_PATH "/usr/lib/${OHOS_TOOLCHAIN_NAME}")
set(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN   "${TOOLCHAIN_ROOT_PATH}")
set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN "${TOOLCHAIN_ROOT_PATH}")
set(CMAKE_ASM_COMPILER_EXTERNAL_TOOLCHAIN "${TOOLCHAIN_ROOT_PATH}")
set(CMAKE_C_COMPILER "${TOOLCHAIN_BIN_PATH}/clang${HOST_SYSTEM_EXE_SUFFIX}")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_BIN_PATH}/clang++${HOST_SYSTEM_EXE_SUFFIX}")

set(OHOS_AR "${TOOLCHAIN_BIN_PATH}/llvm-ar${HOST_SYSTEM_EXE_SUFFIX}")
set(OHOS_RANLIB "${TOOLCHAIN_BIN_PATH}/llvm-ranlib${HOST_SYSTEM_EXE_SUFFIX}")
set(CMAKE_AR                "${OHOS_AR}" CACHE FILEPATH "Archiver")
set(CMAKE_RANLIB            "${OHOS_RANLIB}" CACHE FILEPATH "Ranlib")
