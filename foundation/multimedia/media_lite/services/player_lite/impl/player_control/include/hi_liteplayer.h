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

#ifndef HI_LITEPLAYER_H
#define HI_LITEPLAYER_H

#include "hi_demuxer.h"

/** \addtogroup     PLAYER */
/** @{ */ /** <!-- [PLAYER] */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


/** Error information of the player */
enum PlayerControlError {
    PLAYERCONTROL_ERROR_VID_PLAY_FAIL = 0x0, /* The video fails to be played. */
    PLAYERCONTROL_ERROR_AUD_PLAY_FAIL,       /* The audio fails to be played. */
    PLAYERCONTROL_ERROR_DEMUX_FAIL,          /* The file fails to be played. */
    PLAYERCONTROL_ERROR_TIMEOUT,             /* Operation timeout. For example, reading data timeout. */
    PLAYERCONTROL_ERROR_NOT_SUPPORT,         /* The file format is not supportted. */
    PLAYERCONTROL_ERROR_UNKNOW,              /* Unknown error. */
    PLAYERCONTROL_ERROR_ILLEGAL_STATEACTION, /* illegal action at cur state. */
    PLAYERCONTROL_ERROR_BUTT,
};

enum PlayerControlEvent {
    PLAYERCONTROL_EVENT_STATE_CHANGED = 0x0, /* the player status changed */
    PLAYERCONTROL_EVENT_EOF,                 /* the player is playing the end */
    PLAYERCONTROL_EVENT_SOF,                 /* the player backward tplay to the start of file */
    PLAYERCONTROL_EVENT_PROGRESS,   /* current playing progress. it will be called every one second.
                                              the additional value that in the unit of ms is current playing time */
    PLAYERCONTROL_EVENT_SEEK_END,   /* seek time jump, the additional value is the seek value */
    PLAYERCONTROL_EVENT_ERROR,               /* play error */
    PLAYERCONTROL_FIRST_VIDEO_FRAME,
    PLAYERCONTROL_FIRST_AUDIO_FRAME,
    PLAYERCONTROL_EVENT_BUTT
};

struct PlayerControlStreamAttr {
    int32_t s32VidStreamId; /**< Video stream ID. */
    int32_t s32AudStreamId; /**< Audio stream ID. */
};

struct PlayerControlParam {
    uint32_t u32PlayPosNotifyIntervalMs; /* Current playing Time notify interval in Ms. */
    uint32_t u32VideoEsBufSize; /* video es stream cache buf size.[1k,200M] unit:byte;
                                    if 0,use the inner default value */
    uint32_t u32AudioEsBufSize; /* audio es stream cache buf size.[1k,100M] unit:byte;
                                    if 0,use the inner default value */
    uint32_t u32VdecFrameBufCnt;  /* Number of VB blocks for storing decoded image frames;
                                                       if 0,use the inner default value */
    bool bPauseMode; /* pause after first frame render mode flag, HI_FALSE:do not pause,
                                           HI_TRUE:pause after first frame render. */
};

typedef void (*PlayerCtrlEeventFun)(void *player, PlayerControlEvent enEvent, const void *data);

struct PlayerCtrlCallbackParam {
    void *player;
    PlayerCtrlEeventFun callbackFun;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
