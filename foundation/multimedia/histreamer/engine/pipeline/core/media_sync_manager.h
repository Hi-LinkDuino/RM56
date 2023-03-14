/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PIPELINE_CORE_PIPELINE_CLOCK_H
#define HISTREAMER_PIPELINE_CORE_PIPELINE_CLOCK_H
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "error_code.h"
#include "foundation/osal/thread/mutex.h"
#include "i_media_sync_center.h"
#include "plugin/common/plugin_time.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class MediaSyncManager : public IMediaSyncCenter, public std::enable_shared_from_this<MediaSyncManager> {
public:
    MediaSyncManager() = default;
    virtual ~MediaSyncManager();
    // interfaces called by hiplayer hirecoder etc.
    ErrorCode SetPlaybackRate(float rate);
    float GetPlaybackRate();
    void WaitAllPrerolled(bool prerolled = true);
    ErrorCode Resume();
    ErrorCode Pause();
    ErrorCode Seek(int64_t mediaTime);
    ErrorCode Reset();

    // interfaces from IMediaSyncCenter
    void AddSynchronizer(IMediaSynchronizer* syncer) override;

    void RemoveSynchronizer(IMediaSynchronizer* syncer) override;
    /**
     * anchor a media time(pts) with real clock time.
     *
     * @param clockTime based on HST_TIME_BASE
     * @param mediaTime media time based on HST_TIME_BASE
     * @param supplier which report this time anchor
     * @retval error code
     */
    ErrorCode UpdateTimeAnchor(int64_t clockTime, int64_t mediaTime, IMediaSynchronizer* supplier) override;

    /**
     * get media time currently
     * @return media time now
     */
    int64_t GetMediaTimeNow() override;

    /***
     * get clock time now
     * @return return clock time based on HST_TIME_BASE
     */
    int64_t GetClockTimeNow() override;

    /**
     * Get clock time anchored with pts
     *
     * @param mediaTime target pts
     * @return clock time anchored with pts
     */
    int64_t GetClockTime(int64_t mediaTime) override;

    /**
     * after IMediaSynchronizer has received the first frame, it should call this function to report the receiving of
     * the first frame.
     *
     * @param supplier which report first frame
     */
    void ReportPrerolled(IMediaSynchronizer* supplier) override;

    void SetMediaTimeRangeEnd(int64_t endMediaTime, int32_t trackId) override;

    void SetMediaTimeRangeStart(int64_t startMediaTime, int32_t trackId) override;

private:
    enum class State {
        RESUMED,
        PAUSED,
    };
    static int64_t GetSystemClock();
    static int64_t SimpleGetMediaTime(int64_t anchorClockTime, int64_t nowClockTime, int64_t anchorMediaTime,
                                      float playRate);
    static int64_t SimpleGetClockTime(int64_t anchorClockTime, int64_t nowMediaTime, int64_t anchorMediaTime,
                                      float playRate);

    bool IsSupplierValid(IMediaSynchronizer* supplier);

    void SimpleUpdateTimeAnchor(int64_t clockTime, int64_t mediaTime);
    void SimpleUpdatePlayRate(float playRate);
    void SetMediaTimeStartEnd(int32_t trackId, int32_t index, int64_t val);
    void SetAllSyncShouldWaitNoLock();
    void ResetTimeAnchorNoLock();

    int64_t ClipMediaTime(int64_t inTime);
    OSAL::Mutex clockMutex_ {};
    State clockState_ {State::PAUSED};
    int8_t currentSyncerPriority_ {IMediaSynchronizer::NONE};
    int64_t currentAnchorClockTime_ {HST_TIME_NONE};
    int64_t currentAnchorMediaTime_ {HST_TIME_NONE};
    int64_t pausedMediaTime_ {HST_TIME_NONE};
    int64_t pausedClockTime_ {HST_TIME_NONE};

    float playRate_ {1.0f};
    bool alreadySetSyncersShouldWait_ {false};
    bool allSyncerShouldPrerolled_ {true};
    bool isSeeking_ {false};
    int64_t seekingMediaTime_ {HST_TIME_NONE};

    // trackid start end
    std::vector<std::tuple<int32_t, int64_t, int64_t>> trackMediaTimeRange_ {};
    int64_t minRangeStartOfMediaTime_ {HST_TIME_NONE};
    int64_t maxRangeEndOfMediaTime_ {HST_TIME_NONE};

    OSAL::Mutex syncersMutex_ {};
    std::vector<IMediaSynchronizer*> syncers_;
    std::vector<IMediaSynchronizer*> prerolledSyncers_;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PIPELINE_CORE_PIPELINE_CLOCK_H
