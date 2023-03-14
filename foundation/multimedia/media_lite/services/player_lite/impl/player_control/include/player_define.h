/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef PLAYER_DEFINE_H
#define PLAYER_DEFINE_H

#include <stdint.h>
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef HI_SUCCESS
#define HI_SUCCESS 0
#endif

#ifndef HI_FAILURE
#define HI_FAILURE -1
#endif

#define MAX_AUD_DECODER_PATH_LEN  256

typedef enum {
    MEDIA_TYPE_VIDEO,
    MEDIA_TYPE_VIDEO_AUDIO,
    MEDIA_TYPE_IMAGE,
    MEDIA_TYPE_AUDIO,
    MEDIA_TYPE_BUTT
} MediaType;

typedef enum {
    DATA_VIDEO,
    DATA_AUDIO,
    DATA_SUB,
    DATA_BUTT
} DataType;

typedef enum {
    EVNET_ON_STATE_CHANGE,
    EVNET_FIRST_VIDEO_REND,
    EVNET_VIDEO_PTS_ERRGAP,
    EVNET_AUDIO_PTS_ERRGAP,
    EVNET_VIDEO_RUNNING_ERR,
    EVNET_AUDIO_RUNNING_ERR,
    EVNET_VIDEO_PLAY_EOS,
    EVNET_VIDEO_PLAY_SOS,
    EVNET_AUDIO_PLAY_EOS,
    EVNET_ON_JPEG_FRAME_RENDED,
    EVNET_FRAME_CONTINUE_LOST,
    EVNET_FIRST_AUDIO_REND,
    EVNET_BUTT
} EventCbType;

typedef enum {
    TPLAY_DIRECT_FORWARD,
    TPLAY_DIRECT_BACKWARD,
    TPLAY_DIRECT_BUTT
} TplayDirect;

typedef enum {
    PLAY_STATUS_IDLE = 0,
    PLAY_STATUS_INIT,
    PLAY_STATUS_PREPARED,
    PLAY_STATUS_PLAY,
    PLAY_STATUS_TPLAY,
    PLAY_STATUS_PAUSE,
    PLAY_STATUS_ERR,
    PLAY_STATUS_BUTT
} PlayerStatus;

typedef enum {
    RENDER_MODE_NORMAL = 0,
    RENDER_MODE_PAUSE_AFTER_PLAY, /* pause after play the first frame */
    RENDER_MODE_BUTT
} RenderMode;

typedef struct {
    int32_t type;
    uint32_t maxWidth;
    uint32_t maxHeight;
    uint32_t frameBufCnt;
    uint32_t errRecover;
} PlayControlVdecAttr;

typedef struct {
    int32_t type;
    uint32_t sampleRate;
    uint32_t channel;
    uint32_t bitWidth; /* bits */
} PlayControlAdecAttr;

typedef struct {
    RenderMode playMode;
    uint32_t videoBufSize;
    uint32_t audioBufSize;
} PlayControlStreamAttr;

struct PlayVideoStreamInfo {
    uint32_t decWidth;
    uint32_t decHeight;
    uint32_t fpsInteger;
    uint32_t fpsDecimal;
    uint32_t freeEsBufSize;
    uint32_t esBufSize;
};

struct PlayAudioStreamInfo {
    uint32_t sampleRate;
    uint32_t channel; /* only support 1-2 */
    uint32_t bitwith;
    uint32_t freeEsBufSize;
    uint32_t esBufSize;
};

struct PlaySyncStatus {
    int64_t firstVidPts;
    int64_t lastVidPts;
    int64_t firstAudPts;
    int64_t lastAudPts;
    int64_t localTime;
    int64_t diffTime;
};

typedef enum {
    DECODER_STATUS_IDLE = 0,
    DECODER_STATUS_PLAY,
    DECODER_STATUS_TPLAY,
    DECODER_STATUS_PAUSE,
    DECODER_STATUS_ERR,
    DECODER_STATUS_BUTT
} DecoderStatus;

struct PlayStatusInfo {
    DecoderStatus vidStatus;
    DecoderStatus audStatus;
    PlaySyncStatus syncStatus;
    uint64_t vidFrameCount;
    uint64_t audFrameCount;
    uint64_t nonDecVidCnt;
};

struct PlayerStreamInfo {
    PlayVideoStreamInfo videoInfo;
    PlayAudioStreamInfo audioInfo;
    PlayStatusInfo avStatus;
};

struct TplayAttr {
    TplayDirect direction;
    float speed;
};

struct VolumeAttr {
    float leftVolume;
    float rightVolume;
};

enum PlaySpeed {
    PLAY_SPEED_BASE = 1,
    PLAY_SPEED_2X_FAST = 2,     /* 2 x speed fast */
    PLAY_SPEED_4X_FAST = 4,     /* 4 x speed fast */
    PLAY_SPEED_8X_FAST = 8,     /* 8 x speed fast */
    PLAY_SPEED_16X_FAST = 16,   /* 16 x speed fast */
    PLAY_SPEED_32X_FAST = 32,   /* 32 x speed fast */
    PLAY_SPEED_64X_FAST = 64,   /* 64 x speed fast */
    PLAY_SPEED_128X_FAST = 128, /* 128 x speed fast */
    PLAY_SPEED_BUTT             /* Invalid speed value */
};

enum PlayerInvoke{
    INVOKE_ENABLE_PAUSE_AFTER_PLAYER = 0x100,
};

struct InvokeParameter{
    PlayerInvoke id;
    void* param;
};

typedef int32_t (*OnEventCallback)(void* priv, EventCbType event, int32_t ext1, int32_t ext2);

typedef struct {
    OnEventCallback onEventCallback;
    void* priv;
} PlayEventCallback;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLAYER_DEFINE_H */
