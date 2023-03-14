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

#define HST_LOG_TAG "MediaSyncManager"

#include "media_sync_manager.h"

#include <algorithm>
#include <cmath>
#include "foundation/log.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "plugin/common/plugin_time.h"
#include "utils/steady_clock.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
namespace {
#define MEDIA_TUPLE_START_INDEX 1
#define MEDIA_TUPLE_END_INDEX 2
}

MediaSyncManager::~MediaSyncManager()
{
    MEDIA_LOG_I("~MediaSyncManager enter.");
}

void MediaSyncManager::AddSynchronizer(IMediaSynchronizer* syncer)
{
    if (syncer != nullptr) {
        OSAL::ScopedLock lock(syncersMutex_);
        if (std::find(syncers_.begin(), syncers_.end(), syncer) != syncers_.end()) {
            return;
        }
        syncers_.emplace_back(syncer);
        MEDIA_LOG_I("add syncer supplier " PUBLIC_LOG_S, syncer->GetSynchronizerName().c_str());
    }
}

void MediaSyncManager::RemoveSynchronizer(IMediaSynchronizer* syncer)
{
    if (syncer != nullptr) {
        OSAL::ScopedLock lock(syncersMutex_);
        auto ite = std::find(syncers_.begin(), syncers_.end(), syncer);
        if (ite != syncers_.end()) {
            syncers_.erase(ite);
            MEDIA_LOG_I("remove syncer supplier " PUBLIC_LOG_S, syncer->GetSynchronizerName().c_str());
        }
    }
}

ErrorCode MediaSyncManager::SetPlaybackRate(float rate)
{
    if (rate < 0) {
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    OSAL::ScopedLock lock(clockMutex_);
    MEDIA_LOG_I("set play rate " PUBLIC_LOG_F, rate);
    if (currentAnchorClockTime_ == HST_TIME_NONE || currentAnchorMediaTime_ == HST_TIME_NONE) {
        SimpleUpdatePlayRate(rate);
        return ErrorCode::SUCCESS;
    }
    int64_t now = GetSystemClock();
    int64_t currentMedia = SimpleGetMediaTime(currentAnchorClockTime_, now, currentAnchorMediaTime_, playRate_);
    SimpleUpdateTimeAnchor(now, currentMedia);
    SimpleUpdatePlayRate(rate);
    return ErrorCode::SUCCESS;
}

float MediaSyncManager::GetPlaybackRate()
{
    OSAL::ScopedLock lock(clockMutex_);
    return playRate_;
}
void MediaSyncManager::SetMediaTimeStartEnd(int32_t trackId, int32_t index, int64_t val)
{
    auto target = std::find_if(trackMediaTimeRange_.begin(), trackMediaTimeRange_.end(),
                               [&trackId] (const std::tuple<int32_t, int64_t, int64_t>& item) -> bool {
        return std::get<0>(item) == trackId;
    });
    if (target == trackMediaTimeRange_.end()) {
        trackMediaTimeRange_.emplace_back(
            std::tuple<int32_t, int64_t, int64_t>(trackId, HST_TIME_NONE, HST_TIME_NONE));
        if (index == MEDIA_TUPLE_START_INDEX) {
            std::get<MEDIA_TUPLE_START_INDEX>(*trackMediaTimeRange_.rbegin()) = val;
        } else if (index == MEDIA_TUPLE_END_INDEX) {
            std::get<MEDIA_TUPLE_END_INDEX>(*trackMediaTimeRange_.rbegin()) = val;
        } else {
            MEDIA_LOG_W("invalid index");
        }
    } else {
        if (index == MEDIA_TUPLE_START_INDEX) {
            std::get<MEDIA_TUPLE_START_INDEX>(*target) = val;
        } else if (index == MEDIA_TUPLE_END_INDEX) {
            std::get<MEDIA_TUPLE_END_INDEX>(*target) = val;
        } else {
            MEDIA_LOG_W("invalid index");
        }
    }
}

void MediaSyncManager::SetMediaTimeRangeStart(int64_t startMediaTime, int32_t trackId)
{
    OSAL::ScopedLock lock(clockMutex_);
    SetMediaTimeStartEnd(trackId, MEDIA_TUPLE_START_INDEX, startMediaTime);
    if (minRangeStartOfMediaTime_ == HST_TIME_NONE || startMediaTime < minRangeStartOfMediaTime_) {
        minRangeStartOfMediaTime_ = startMediaTime;
        MEDIA_LOG_I("set media started at " PUBLIC_LOG_D64, minRangeStartOfMediaTime_);
    }
}
void MediaSyncManager::SetMediaTimeRangeEnd(int64_t endMediaTime, int32_t trackId)
{
    OSAL::ScopedLock lock(clockMutex_);
    SetMediaTimeStartEnd(trackId, MEDIA_TUPLE_END_INDEX, endMediaTime);
    if (maxRangeEndOfMediaTime_ == HST_TIME_NONE || endMediaTime > maxRangeEndOfMediaTime_) {
        maxRangeEndOfMediaTime_ = endMediaTime;
        MEDIA_LOG_I("set media end at " PUBLIC_LOG_D64, maxRangeEndOfMediaTime_);
    }
}

void MediaSyncManager::WaitAllPrerolled(bool prerolled)
{
    allSyncerShouldPrerolled_ = prerolled;
}

void MediaSyncManager::SetAllSyncShouldWaitNoLock()
{
    if (allSyncerShouldPrerolled_ && !alreadySetSyncersShouldWait_) {
        prerolledSyncers_.clear();
        {
            OSAL::ScopedLock lock1(syncersMutex_);
            if (syncers_.size() > 1) {
                for (const auto &supplier: syncers_) {
                    supplier->WaitAllPrerolled(true);
                }
            }
        }
        alreadySetSyncersShouldWait_ = true;
    }
}

ErrorCode MediaSyncManager::Resume()
{
    OSAL::ScopedLock lock(clockMutex_);
    // update time anchor after a pause during normal playing
    if (clockState_ == State::PAUSED && pausedMediaTime_ != HST_TIME_NONE && alreadySetSyncersShouldWait_) {
        SimpleUpdateTimeAnchor(GetSystemClock(), pausedMediaTime_);
        pausedMediaTime_ = HST_TIME_NONE;
        pausedClockTime_ = HST_TIME_NONE;
    }
    if (clockState_ == State::RESUMED) {
        return ErrorCode::SUCCESS;
    }
    SetAllSyncShouldWaitNoLock();
    MEDIA_LOG_I("resume");
    clockState_ = State::RESUMED;
    return ErrorCode::SUCCESS;
}
int64_t MediaSyncManager::GetSystemClock()
{
    auto tmp = SteadyClock::GetCurrentTimeNanoSec();
    int64_t hstTime = 0;
    if (!Plugin::Ns2HstTime(tmp, hstTime)) {
        return HST_TIME_NONE;
    }
    return hstTime;
}
ErrorCode MediaSyncManager::Pause()
{
    OSAL::ScopedLock lock(clockMutex_);
    if (clockState_ == State::PAUSED) {
        return ErrorCode::SUCCESS;
    }
    pausedClockTime_ = GetSystemClock();
    if (currentAnchorMediaTime_ != HST_TIME_NONE && currentAnchorClockTime_ != HST_TIME_NONE) {
        pausedMediaTime_ = SimpleGetMediaTime(currentAnchorClockTime_, pausedClockTime_, currentAnchorMediaTime_,
                                              playRate_);
    } else {
        pausedMediaTime_ = HST_TIME_NONE;
    }
    pausedMediaTime_ = ClipMediaTime(pausedMediaTime_);
    MEDIA_LOG_I("pause with clockTime " PUBLIC_LOG_D64 ", mediaTime " PUBLIC_LOG_D64, pausedClockTime_,
                pausedMediaTime_);
    clockState_ = State::PAUSED;
    return ErrorCode::SUCCESS;
}

ErrorCode MediaSyncManager::Seek(int64_t mediaTime)
{
    OSAL::ScopedLock lock(clockMutex_);
    if (minRangeStartOfMediaTime_ == HST_TIME_NONE || maxRangeEndOfMediaTime_ == HST_TIME_NONE) {
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    if (mediaTime > maxRangeEndOfMediaTime_ || mediaTime < minRangeStartOfMediaTime_) {
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    isSeeking_ = true;
    seekingMediaTime_ = mediaTime;
    alreadySetSyncersShouldWait_ = false; // set already as false
    SetAllSyncShouldWaitNoLock(); // all suppliers should sync preroll again after seek
    ResetTimeAnchorNoLock(); // reset the time anchor
    return ErrorCode::SUCCESS;
}

ErrorCode MediaSyncManager::Reset()
{
    MEDIA_LOG_I("do Reset");
    OSAL::ScopedLock lock(clockMutex_);
    clockState_ = State::PAUSED;
    ResetTimeAnchorNoLock();
    pausedClockTime_ = HST_TIME_NONE;
    playRate_ = 1.0f;
    alreadySetSyncersShouldWait_ = false;
    allSyncerShouldPrerolled_ = true;
    isSeeking_ = false;
    seekingMediaTime_ = HST_TIME_NONE;
    trackMediaTimeRange_.clear();
    minRangeStartOfMediaTime_ = HST_TIME_NONE;
    maxRangeEndOfMediaTime_ = HST_TIME_NONE;
    {
        OSAL::ScopedLock lock1(syncersMutex_);
        syncers_.clear();
        prerolledSyncers_.clear();
    }
    return ErrorCode::SUCCESS;
}

int64_t MediaSyncManager::ClipMediaTime(int64_t inTime)
{
    int64_t ret = inTime;
    if (minRangeStartOfMediaTime_ != HST_TIME_NONE && ret < minRangeStartOfMediaTime_) {
        ret = minRangeStartOfMediaTime_;
        MEDIA_LOG_D("clip to min media time " PUBLIC_LOG_D64, ret);
    }
    if (maxRangeEndOfMediaTime_ != HST_TIME_NONE && ret > maxRangeEndOfMediaTime_) {
        ret = maxRangeEndOfMediaTime_;
        MEDIA_LOG_D("clip to max media time " PUBLIC_LOG_D64, ret);
    }
    return ret;
}

void MediaSyncManager::ResetTimeAnchorNoLock()
{
    // todo we should let suppliers wait again?
    pausedMediaTime_ = HST_TIME_NONE;
    currentSyncerPriority_ = IMediaSynchronizer::NONE;
    SimpleUpdateTimeAnchor(HST_TIME_NONE, HST_TIME_NONE);
}

void MediaSyncManager::SimpleUpdatePlayRate(float playRate)
{
    playRate_ = playRate;
}

void MediaSyncManager::SimpleUpdateTimeAnchor(int64_t clockTime, int64_t mediaTime)
{
    currentAnchorMediaTime_ = mediaTime;
    currentAnchorClockTime_ = clockTime;
}
bool MediaSyncManager::IsSupplierValid(IMediaSynchronizer* supplier)
{
    OSAL::ScopedLock lock(syncersMutex_);
    return std::find(syncers_.begin(), syncers_.end(), supplier) != syncers_.end();
}
ErrorCode MediaSyncManager::UpdateTimeAnchor(int64_t clockTime, int64_t mediaTime, IMediaSynchronizer* supplier)
{
    OSAL::ScopedLock lock(clockMutex_);
    if (clockTime == HST_TIME_NONE || mediaTime == HST_TIME_NONE || supplier == nullptr) {
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    if (IsSupplierValid(supplier) && supplier->GetPriority() >= currentSyncerPriority_) {
        currentSyncerPriority_ = supplier->GetPriority();
        SimpleUpdateTimeAnchor(clockTime, mediaTime);
        MEDIA_LOG_DD("update time anchor to priority " PUBLIC_LOG_D32 ", mediaTime " PUBLIC_LOG_D64 ", clockTime "
        PUBLIC_LOG_D64, currentSyncerPriority_, currentAnchorMediaTime_, currentAnchorClockTime_);
    }
    if (isSeeking_ && seekingMediaTime_ <= currentAnchorMediaTime_)  {
        MEDIA_LOG_I("leaving seeking");
        isSeeking_ = false;
    }
    return ErrorCode::SUCCESS;
}
int64_t MediaSyncManager::SimpleGetMediaTime(int64_t anchorClockTime, int64_t nowClockTime, int64_t anchorMediaTime,
                                             float playRate)
{
    if (std::fabs(playRate - 0) < 1e-9) { // 0 threshold
        return HST_TIME_NONE;
    }
    if (anchorClockTime == HST_TIME_NONE || nowClockTime == HST_TIME_NONE || anchorMediaTime == HST_TIME_NONE) {
        return HST_TIME_NONE;
    }
    return anchorMediaTime + (nowClockTime - anchorClockTime) * static_cast<double>(playRate);
}
int64_t MediaSyncManager::GetMediaTimeNow()
{
    OSAL::ScopedLock lock(clockMutex_);
    if (isSeeking_) {
        return seekingMediaTime_;
    }
    if (clockState_ == State::PAUSED) {
        if (pausedMediaTime_ == HST_TIME_NONE) {
            return 0;
        }
        return pausedMediaTime_;
    }
    auto ret = SimpleGetMediaTime(currentAnchorClockTime_, GetSystemClock(), currentAnchorMediaTime_, playRate_);
    // clip into min&max media time
    return ClipMediaTime(ret);
}

int64_t MediaSyncManager::GetClockTimeNow()
{
    {
        OSAL::ScopedLock lock(clockMutex_);
        if (clockState_ == State::PAUSED) {
            return pausedClockTime_;
        }
    }
    return GetSystemClock();
}
int64_t MediaSyncManager::SimpleGetClockTime(int64_t anchorClockTime, int64_t nowMediaTime, int64_t anchorMediaTime,
                                             float playRate)
{
    if (std::fabs(playRate - 0) < 1e-9) { // 0 threshold
        return HST_TIME_NONE;
    }
    if (anchorClockTime == HST_TIME_NONE || nowMediaTime == HST_TIME_NONE || anchorMediaTime == HST_TIME_NONE) {
        return HST_TIME_NONE;
    }
    return anchorClockTime + (nowMediaTime - anchorMediaTime) / static_cast<double>(playRate);
}
int64_t MediaSyncManager::GetClockTime(int64_t mediaTime)
{
    OSAL::ScopedLock lock(clockMutex_);
    if (minRangeStartOfMediaTime_ != HST_TIME_NONE && mediaTime < minRangeStartOfMediaTime_) {
        MEDIA_LOG_W("media time " PUBLIC_LOG_D64 " less than min media time " PUBLIC_LOG_D64,
                    mediaTime, minRangeStartOfMediaTime_);
    }
    if (maxRangeEndOfMediaTime_ != HST_TIME_NONE && mediaTime > maxRangeEndOfMediaTime_) {
        MEDIA_LOG_W("media time " PUBLIC_LOG_D64 " exceed max media time " PUBLIC_LOG_D64,
                    mediaTime, maxRangeEndOfMediaTime_);
    }
    return SimpleGetClockTime(currentAnchorClockTime_, mediaTime, currentAnchorMediaTime_, playRate_);
}

void MediaSyncManager::ReportPrerolled(IMediaSynchronizer* supplier)
{
    if (supplier == nullptr) {
        return;
    }
    if (!allSyncerShouldPrerolled_) {
        return;
    }
    OSAL::ScopedLock lock(syncersMutex_);
    auto ite = std::find(prerolledSyncers_.begin(), prerolledSyncers_.end(), supplier);
    if (ite != prerolledSyncers_.end()) {
        MEDIA_LOG_I("supplier already reported prerolled");
        return;
    }
    prerolledSyncers_.emplace_back(supplier);
    if (prerolledSyncers_.size() == syncers_.size()) {
        for (const auto& prerolled : prerolledSyncers_) {
            prerolled->NotifyAllPrerolled();
        }
        prerolledSyncers_.clear();
    }
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS