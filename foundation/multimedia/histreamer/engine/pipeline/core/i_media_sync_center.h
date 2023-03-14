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

#ifndef HISTREAMER_PIPELINE_CORE_I_MEDIA_SYNC_CENTER_H
#define HISTREAMER_PIPELINE_CORE_I_MEDIA_SYNC_CENTER_H
namespace OHOS {
namespace Media {
namespace Pipeline {
/**
 * @brief Pipeline clock supplier
 */
struct IMediaSynchronizer {
    const static int8_t NONE = -1;
    const static int8_t VIDEO_SINK = 0;
    const static int8_t AUDIO_SINK = 2;
    const static int8_t VIDEO_SRC = 4;
    const static int8_t AUDIO_SRC = 6;
    virtual ~IMediaSynchronizer() = default;
    virtual int8_t GetPriority() = 0;
    virtual std::string GetSynchronizerName() = 0;
    virtual void WaitAllPrerolled(bool shouldWait) = 0;
    virtual void NotifyAllPrerolled() = 0;
};

struct IMediaSyncCenter {
    virtual void AddSynchronizer(IMediaSynchronizer* syncer) = 0;

    virtual void RemoveSynchronizer(IMediaSynchronizer* syncer) = 0;
    /**
     * anchor a media time(pts) with real clock time.
     *
     * @param clockTime based on HST_TIME_BASE
     * @param mediaTime media time based on HST_TIME_BASE
     * @param supplier which report this time anchor
     * @retval error code
     */
    virtual ErrorCode UpdateTimeAnchor(int64_t clockTime, int64_t mediaTime, IMediaSynchronizer* supplier) = 0;

    /**
     * get media time currently
     * @return media time now
     */
    virtual int64_t GetMediaTimeNow() = 0;

    /***
     * get clock time now
     * @return return clock time based on HST_TIME_BASE
     */
    virtual int64_t GetClockTimeNow() = 0;

    /**
     * Get clock time anchored with pts
     *
     * @param mediaTime target pts
     * @return clock time anchored with pts
     */
    virtual int64_t GetClockTime(int64_t mediaTime) = 0;

    /**
     * after IMediaSynchronizer has received the first frame, it should call this function to report the receiving of
     * the first frame.
     *
     * @param supplier which report first frame
     */
    virtual void ReportPrerolled(IMediaSynchronizer* supplier) = 0;

    virtual void SetMediaTimeRangeStart(int64_t startMediaTime, int32_t trackId) = 0;

    virtual void SetMediaTimeRangeEnd(int64_t endMediaTime, int32_t trackId) = 0;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PIPELINE_CORE_I_MEDIA_SYNC_CENTER_H
