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

set(GlobalSrc "${PROJECT_SOURCE_DIR}/base/global/resmgr_lite/frameworks/resmgr_lite/src")
set(GlobalInc "${PROJECT_SOURCE_DIR}/base/global/resmgr_lite/frameworks/resmgr_lite/include")
set(GlobalKitInc "${PROJECT_SOURCE_DIR}/base/global/resmgr_lite/interfaces/innerkits/include")

include_directories(${PROJECT_SOURCE_DIR}/third_party/bounds_checking_function/include)

add_library(global_resmgr STATIC
    ${GlobalSrc}/global.c
    ${GlobalSrc}/global_utils.c
)

target_include_directories(global_resmgr PUBLIC ${GlobalInc} ${GlobalKitInc})