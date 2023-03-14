# Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

CMAKE_MINIMUM_REQUIRED(VERSION 3.13...3.20)

PROJECT(histreamer_tests)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(
    ${TOP_DIR}/../3rdparty/ohos/foundation/multimedia/media_lite/interfaces/innerkits
    ${TOP_DIR}/../3rdparty/ohos/foundation/multimedia/utils/lite/interfaces/kits
    ${TOP_DIR}/engine/scene/player/lite
)

# Must comment out these two lines to compile standard player
# But must uncomment them to compile histreamer_tests
#unset(PLAYER_LITE CACHE)
#option(PLAYER_LITE "tests should use player lite interface" ON)

if (NOT MINGW)
    # support findSymbol, cmake >= 3.13
    # ADD_LINK_OPTIONS( -rdynamic)
endif ()

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin) 

ADD_SUBDIRECTORY(ut)
ADD_SUBDIRECTORY(st)
