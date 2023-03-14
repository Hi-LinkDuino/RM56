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

IF (MSVC)
    ADD_DEFINITIONS(-DMSVC_VMG_ENABLED /std:c++11)  #  /Z7)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /vmg")
ENDIF (MSVC)

#IF(UNIX)
#MESSAGE("UNIX")
#ADD_DEFINITIONS(
#    -g -ggdb
#)
#ENDIF(UNIX)

IF (UNIX AND NOT CYGWIN)
    ADD_DEFINITIONS(
            -fPIC -DPROFILE
    )
    if (${OHOS_LITE})
        set(CMAKE_CXX_FLAGS "-g -ggdb3 -O0 -Wall -Wl,--disable-new-dtags")
    else()
        set(CMAKE_CXX_FLAGS "-g -ggdb3 -O0 -std=c++17 -Wall -Wl,--disable-new-dtags")
    endif()

ENDIF (UNIX AND NOT CYGWIN)

ADD_DEFINITIONS(
        -D__STDC_FORMAT_MACROS
        -DHST_PLUGIN_PATH="./"
)
if (WIN32)
add_definitions( -DHST_PLUGIN_FILE_TAIL=".dll" )
else()
add_definitions( -DHST_PLUGIN_FILE_TAIL=".so" )
endif()

IF (CMAKE_CL_64)
    ADD_DEFINITIONS(-DWIN64)
ENDIF (CMAKE_CL_64)

######################################################
# include directories
include_directories(
        ${TOP_DIR}/engine
        ${TOP_DIR}/engine/scene/player
        ${TOP_DIR}/engine/scene/recorder
        ${TOP_DIR}/engine/pipeline
        ${TOP_DIR}/engine/pipeline/core
        ${TOP_DIR}/engine/foundation
        ${TOP_DIR}/engine/foundation/osal
        ${TOP_DIR}/engine/utils
        ${TOP_DIR}/engine/pipeline/filters
        ${TOP_DIR}/engine/pipeline/filters/player
        ${TOP_DIR}/engine/plugin
)

if (AVS3DA_ENABLE)
    include_directories(
        ${HISTREAMER_PLUGINS_DIR}/codec/avs3_audio_decoder/avs3_codec/libavs3_common
        ${HISTREAMER_PLUGINS_DIR}/codec/avs3_audio_decoder/avs3_codec/avs3Decoder/include
        ${HISTREAMER_PLUGINS_DIR}/codec/avs3_audio_decoder/avs3_render
    )
endif ()

if (NOT ${OHOS_LITE})
    message(STATUS "media interface header included.")
    include_directories(
            ${TOP_DIR}/engine/scene/standard
    )
endif ()

######################################################

file(GLOB_RECURSE HISTREAMER_BASE_SRCS
        ${TOP_DIR}/engine/pipeline/*.cpp
        ${TOP_DIR}/engine/plugin/common/*.cpp
        ${TOP_DIR}/engine/plugin/core/*.cpp
        ${TOP_DIR}/engine/plugin/types/*.cpp
        ${TOP_DIR}/engine/scene/player/internal/*.cpp
        ${TOP_DIR}/engine/scene/recorder/internal/*.cpp
        ${TOP_DIR}/engine/scene/common/*.cpp
        ${TOP_DIR}/engine/foundation/*.cpp
        ${TOP_DIR}/engine/utils/*.cpp
        )

file(GLOB_RECURSE COMMON_PLUGIN_SRCS
        ${TOP_DIR}/engine/plugin/plugins/demuxer/wav_demuxer/*.cpp
        ${TOP_DIR}/engine/plugin/plugins/ffmpeg_adapter/*.cpp
        ${TOP_DIR}/engine/plugin/plugins/sink/sdl/*.cpp
        ${TOP_DIR}/engine/plugin/plugins/sink/file_sink/*.cpp
        ${TOP_DIR}/engine/plugin/plugins/source/file_source/*.cpp
        ${TOP_DIR}/engine/plugin/plugins/minimp3_adapter/*.cpp
        ${TOP_DIR}/engine/plugin/plugins/minimp3_adapter/*.c
        ${TOP_DIR}/engine/plugin/plugins/source/http_source/*.cpp
        ${TOP_DIR}/engine/plugin/plugins/source/http_source_test/*.cpp
        ${TOP_DIR}/engine/plugin/plugins/source/stream_source/*.cpp
)

if (AVS3DA_ENABLE)
    file(GLOB_RECURSE AVS3DA_PLUGIN_SRCS
        ${HISTREAMER_PLUGINS_DIR}/codec/avs3_audio_decoder/avs3_codec/libavs3_common/*.c
        ${HISTREAMER_PLUGINS_DIR}/codec/avs3_audio_decoder/avs3_codec/avs3Decoder/src/*.c
        ${HISTREAMER_PLUGINS_DIR}/codec/avs3_audio_decoder/avs3_render/*.c
        ${HISTREAMER_PLUGINS_DIR}/codec/avs3_audio_decoder/*.cpp
    )
endif()

if (OHOS_LITE)
    message(STATUS "#1 player lite source")
    file(GLOB_RECURSE PLAYER_SRCS
            ${TOP_DIR}/engine/scene/lite/*.cpp
            ${TOP_DIR}/engine/scene/player/lite/*.cpp
            )
    message(STATUS "#1 recorder lite source")
    file(GLOB_RECURSE RECORDER_SRCS
            ${TOP_DIR}/engine/scene/lite/*.cpp
            ${TOP_DIR}/engine/scene/recorder/lite/*.cpp
            )
else ()
    message(STATUS "player standard source")
    file(GLOB_RECURSE PLAYER_SRCS
            ${TOP_DIR}/engine/scene/standard/*.cpp
            ${TOP_DIR}/engine/scene/player/standard/*.cpp
            )
    message(STATUS "recorder standard source")
    file(GLOB_RECURSE RECORDER_SRCS
            ${TOP_DIR}/engine/scene/standard/*.cpp
            ${TOP_DIR}/engine/scene/recorder/standard/*.cpp
            )
    file(GLOB_RECURSE PLAT_PLUGIN_SRCS
            ${TOP_DIR}/engine/plugin/plugins/source/audio_capture/*.cpp
            ${TOP_DIR}/engine/plugin/plugins/source/video_capture/*.cpp
            )
endif ()

set(HISTREAMER_SRCS ${HISTREAMER_BASE_SRCS} ${PLAYER_SRCS} ${RECORDER_SRCS} ${COMMON_PLUGIN_SRCS} ${AVS3DA_PLUGIN_SRCS} ${PLAT_PLUGIN_SRCS})

file(GLOB_RECURSE PLUGINS_STATIC_BUILD_SRCS
        ${TOP_DIR}/engine/plugin/plugins/source/file_source/*.cpp
        )

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

link_directories(
        ${ffmpeg_lib_path}
        ${sdl_lib_path}
        ${TOP_DIR}/../3rdparty/curl/lib/windows
)

if (WIN32)
    link_libraries(
            pthread
            m
            avcodec
            avformat
            avutil
            avfilter
            swresample
            swscale
            SDL2
            curl
    )
elseif (LINUX_DEMO)
else ()
    link_libraries(
            log
            FFmpeg
            SDL2
    )
endif ()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SHARED_LIBRARY_PREFIX "libmedia_engine_")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".z.so")

add_library(${PROJECT_NAME} SHARED ${HISTREAMER_SRCS} ${PLUGINS_STATIC_BUILD_SRCS}
        ${3RDPARTY_SRCS})
