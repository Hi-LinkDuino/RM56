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

cmake_minimum_required(VERSION 3.16.5)

project(nativeapi_locale)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../resmgr_lite/frameworks/interfaces/innerkits/include
                    ${CMAKE_SOURCE_DIR}/third_party/bounds_checking_function/include)

# source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src LOCALE)

add_library(nativeapi_locale STATIC
    ${LOCALE}
    )
target_link_libraries(nativeapi_locale ui jsfwk)
