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
#define HST_LOG_TAG "MediaSyncSink"
#include "media_synchronous_sink.h"
#include "foundation/log.h"
namespace OHOS {
namespace Media {
namespace Pipeline {
void MediaSynchronousSink::Init(EventReceiver *receiver, FilterCallback *callback)
{
    FilterBase::Init(receiver, callback);
    auto syncCenter = syncCenter_.lock();
    if (syncCenter) {
        syncCenter->AddSynchronizer(this);
    }
}
MediaSynchronousSink::~MediaSynchronousSink()
{
    MEDIA_LOG_I("~MediaSynchronousSink enter .");
    auto syncCenter = syncCenter_.lock();
    if (syncCenter) {
        syncCenter->RemoveSynchronizer(this);
    }
}
void MediaSynchronousSink::WaitAllPrerolled(bool shouldWait)
{
    waitForPrerolled_ = shouldWait;
}
int8_t MediaSynchronousSink::GetPriority()
{
    return syncerPriority_;
}

void MediaSynchronousSink::ResetPrerollReported()
{
    hasReportedPreroll_ = false;
}

ErrorCode MediaSynchronousSink::WriteToPluginRefTimeSync(const AVBufferPtr& buffer)
{
    if (!hasReportedPreroll_) {
        auto syncCenter = syncCenter_.lock();
        if (syncCenter) {
            syncCenter->ReportPrerolled(this);
        }
        hasReportedPreroll_ = true;
    }
    if (waitForPrerolled_) {
        OSAL::ScopedLock lock(prerollMutex_);
        if (!prerollCond_.WaitFor(lock, Plugin::HstTime2Ms(waitPrerolledTimeout_),
            [&] { return waitForPrerolled_.load(); })) {
            MEDIA_LOG_W(PUBLIC_LOG_S " wait for preroll timeout", GetName().c_str());
        }
        // no need to wait for preroll next time
        waitForPrerolled_ = false;
    }
    return DoSyncWrite(buffer);
}

void MediaSynchronousSink::NotifyAllPrerolled()
{
    OSAL::ScopedLock lock(prerollMutex_);
    waitForPrerolled_ = false;
    prerollCond_.NotifyAll();
}

void MediaSynchronousSink::UpdateMediaTimeRange(const Plugin::Meta& meta)
{
    int64_t trackStartTime = 0;
    meta.GetInt64(Plugin::MetaID::MEDIA_START_TIME, trackStartTime);
    uint32_t trackId = 0;
    meta.GetUint32(Plugin::MetaID::TRACK_ID, trackId);
    auto syncCenter = syncCenter_.lock();
    if (syncCenter) {
        syncCenter->SetMediaTimeRangeStart(trackStartTime, trackId);
    }
    int64_t trackDuration = 0;
    if (meta.GetInt64(Plugin::MetaID::MEDIA_DURATION, trackDuration)) {
        if (syncCenter) {
            syncCenter->SetMediaTimeRangeEnd(trackDuration + trackStartTime, trackId);
        }
    } else {
        if (syncCenter) {
            syncCenter->SetMediaTimeRangeEnd(INT64_MAX, trackId);
        }
    }
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS