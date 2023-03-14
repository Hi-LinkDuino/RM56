# Copyright (c) 2020 Huawei Device Co., Ltd.
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

cmake_minimum_required(VERSION 3.1.0)

project(nativeapi_storage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(JSFWK_INTERFACES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../foundation/ace/interfaces/innerkits/builtin")
set(PARAM_INTERFACES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../base/startup/syspara_lite/interfaces/kits")
set(PARAM_FRAMEWORKS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../base/startup/syspara_lite/frameworks")
set(PARAM_HALS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../base/startup/syspara_lite/hals")
set(SECUREC_INTERFACES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../third_party/bounds_checking_function/include")
set(ABILITY_INTERFACES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../foundation/aafwk/aafwk_lite/interfaces/kits/ability_lite")
set(UI_INTERFACES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../foundation/graphic/ui/huawei_proprietary")

# header files
include_directories(
    ${PARAM_INTERFACES_PATH}
    ${SECUREC_INTERFACES_PATH}
    ${ABILITY_INTERFACES_PATH}
    ${PARAM_FRAMEWORKS_PATH}/parameter/src
    ${PARAM_HALS_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deviceinfokit/include
    ${CMAKE_CURRENT_SOURCE_DIR}/filekit/include
    ${CMAKE_CURRENT_SOURCE_DIR}/kvstorekit/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    )

# source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/common/src COMMON)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/deviceinfokit/src DEVICEINFO)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/filekit/src FILEKIT)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/kvstorekit/src KVSTOREKIT)

add_library(utils_jsapi STATIC
    ${PARAM_FRAMEWORKS_PATH}/parameter/src/parameter_common.c
    ${PARAM_FRAMEWORKS_PATH}/parameter/src/param_impl_posix/param_impl_posix.c
    ${COMMON}
    ${DEVICEINFO}
    ${FILEKIT}
    ${KVSTOREKIT}
    )

target_compile_definitions(utils_jsapi PRIVATE INCREMENTAL_VERSION=""
    BUILD_TYPE=""
    BUILD_USER=""
    BUILD_TIME=""
    BUILD_HOST=""
    BUILD_ROOTHASH="")

target_link_libraries(utils_jsapi jsfwk ui)
target_include_directories(utils_jsapi PUBLIC ${ABILITY_INTERFACES_PATH} ${PARAM_INTERFACES_PATH} ${PARAM_HALS_PATH})
