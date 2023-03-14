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

#ifndef PLAYER_SYNC_H
#define PLAYER_SYNC_H

#include <pthread.h>
#include "player_define.h"

namespace OHOS {
namespace Media {
typedef enum SyncRet {
    SYNC_RET_PLAY,
    SYNC_RET_REPEAT,
    SYNC_RET_DROP,
    SYNC_RET_BUTT
} SyncRet;

typedef enum SyncChn {
    SYNC_CHN_VID,
    SYNC_CHN_AUD,
    SYNC_CHN_SUB,
    SYNC_CHN_BUTT
} SyncChn;

typedef struct {
    int64_t firstVidPts;
    int64_t lastVidPts;
    int64_t firstAudPts;
    int64_t lastAudPts;
    int64_t localTime;
    int64_t diffTime;
} PlayerSyncStatus;

typedef enum {
    PLAYER_SYNC_REF_VID,
    PLAYER_SYNC_REF_AUD,
    PLAYER_SYNC_REF_NONE,
    PLAYER_SYNC_REF_BUTT
} PlayerSyncRefType;


typedef struct {
    /* Plus time range during video synchronization */
    uint32_t vidPlusTime;
    /* Negative time range during video synchronization */
    uint32_t vidNegativeTime;
} PlayerSyncRegion;

typedef struct {
    PlayerSyncRefType refType;
    PlayerSyncRegion syncStartRegion;
    PlayerSyncRegion syncStopRegion;
    bool isQuickOutput;
} PlayerSyncAttr;


class PlayerSync {
public:
    PlayerSync();

    virtual ~PlayerSync();
    int32_t Init(void);
    int32_t Deinit(void);
    int32_t ProcVidFrame(int64_t pts, SyncRet &result);
    int32_t ProcAudFrame(int64_t pts, SyncRet &result);
    int32_t Reset(SyncChn syncChn);
    int32_t Stop(SyncChn syncChn);
    int32_t Start(SyncChn syncChn);
    int32_t GetStatus(PlaySyncStatus &info);
    int32_t SetAttr(PlayerSyncAttr &syncAttr);
    int32_t GetAttr(PlayerSyncAttr &syncAttr);
    int32_t SetSpeed(float speed, TplayDirect  tplayDirect);
    int32_t Resume();

private:
    SyncRet CheckAVDiff(int64_t latenessMs, uint32_t &continueLost);
    int32_t TPlayProcess(int64_t timestampUs, SyncRet &syncRet);
    void UpdateCurTimeWithAudio(int64_t& nowUs);
    SyncRet OnVideoFirstFrame(int64_t ptsUs);

private:
    PlayerSyncAttr syncAttr_;
    pthread_mutex_t audSyncLock_;
    pthread_mutex_t vidSyncLock_;
    bool isVidEnable_;
    bool isAudEnable_;
    bool isFristVidFrame_;
    int64_t vidTimeSourceDelta_;
    int64_t lastVideoTsUs_;  // pts in us
    int64_t firstVidFrameTs_;
    int64_t frstVidFrameTime_;
    uint32_t continousVidLost_;  // continous video frame lost count
    bool  isFristAudFrame_;
    int64_t audTimeSourceDelta_;
    int64_t lastAudioTsUs_;      // pts in us
    int64_t lastAudioRealTsUs_;  // clock time in us
    int64_t firstAudFrameTs_;
    int64_t firstAudFrameTime_;
    bool  isInTrickPlayMode_;
    float speed_;
    TplayDirect  tplayDirect_;
    int64_t diffAvRenderTime_;
    bool isInited_;
};
};
};
#endif /* PLAYER_SYNC_H */
