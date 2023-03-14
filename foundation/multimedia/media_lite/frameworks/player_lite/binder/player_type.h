/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef PLAYER_TYPE_H
#define PLAYER_TYPE_H
namespace OHOS {
typedef enum {
    PLAYER_SERVER_SET_SOURCE,
    PLAYER_SERVER_PREPARE,
    PLAYER_SERVER_PLAY,
    PLAYER_SERVER_IS_PLAYING,
    PLAYER_SERVER_PAUSE,
    PLAYER_SERVER_STOP,
    PLAYER_SERVER_REWIND,
    PLAYER_SERVER_SET_VOLUME,
    PLAYER_SERVER_SET_VIDEO_SURFACE,
    PLAYER_SERVER_ENABLE_SINGLE_LOOPING,
    PLAYER_SERVER_IS_SINGLE_LOOPING,
    PLAYER_SERVER_GET_CURRENT_TIME,
    PLAYER_SERVER_GET_DURATION,
    PLAYER_SERVER_GET_VIDEO_WIDTH,
    PLAYER_SERVER_GET_VIDEO_HEIGHT,
    PLAYER_SERVER_RESET,
    PLAYER_SERVER_RELEASE,
    PLAYER_SERVER_SET_PLAYER_CALLBACK,
    PLAYER_SERVER_GET_STATE,
    PLAYER_SERVER_SET_SPEED,
    PLAYER_SERVER_GET_SPEED,
    PLAYER_SERVER_SET_PARAMETER,
    PLAYER_SERVER_SET_AUDIO_STREAM_TYPE,
    PLAYER_SERVER_GET_AUDIO_STREAM_TYPE,
} PlayerServerCall;

typedef enum {
    ON_PALYBACK_COMPLETE,
    ON_ERROR,
    ON_INFO,
    ON_VIDEO_SIZE_CHANGED,
    ON_REWIND_TO_COMPLETE,
    COMMAND_ERROR = 0x7fff
} PlayerClientCall;

struct CallBackPara {
    int funcId;
    int type;
    void* ret;
    void* data;
};

const int32_t DEFAULT_IPC_SIZE = 100;

const char SERVICE_NAME[] = "PlayerServer";
}
#endif
