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

#ifndef HISTREAMER_HIPLAYER_CALLBACKER_LOOPER_H
#define HISTREAMER_HIPLAYER_CALLBACKER_LOOPER_H

#include <list>
#include <utility>

#include "i_player_engine.h"
#include "foundation/osal/thread/task.h"
#include "plugin/common/any.h"
namespace OHOS {
namespace Media {
class HiPlayerCallbackLooper : public IPlayerEngineObs {
public:
    explicit HiPlayerCallbackLooper();
    ~HiPlayerCallbackLooper() override;

    bool IsStarted();

    void Stop();

    void StartWithPlayerEngineObs(const std::weak_ptr<IPlayerEngineObs>& obs);

    void SetPlayEngine(IPlayerEngine* engine);

    void StartReportMediaProgress(int64_t updateIntervalMs = 100);

    void StopReportMediaProgress();

    void ManualReportMediaProgressOnce();

    void OnError(PlayerErrorType errorType, int32_t errorCode) override;

    void OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody) override;

private:

    void LoopOnce();

    void DoReportMediaProgress();
    void DoReportInfo(const Plugin::Any& info);
    void DoReportError(const Plugin::Any& error);

    struct Event{
        Event(int32_t inWhat, int64_t inWhenMs, Plugin::Any inAny): what(inWhat), whenMs(inWhenMs),
            detail(std::move(inAny)) {}
        int32_t what {0};
        int64_t whenMs {INT64_MAX};
        Plugin::Any detail;
    };
    class EventQueue {
    public:
        void Enqueue(const std::shared_ptr<Event>& event);
        std::shared_ptr<Event> Next();
        void Quit();
    private:
        OSAL::Mutex queueMutex_ {};
        OSAL::ConditionVariable queueHeadUpdatedCond_ {};
        std::list<std::shared_ptr<Event>> queue_ {};
        bool quit_ {false};
    };

    OSAL::Task task_;
    bool taskStarted_ {false};
    IPlayerEngine* playerEngine_ {};
    std::weak_ptr<IPlayerEngineObs> obs_ {};
    EventQueue eventQueue_ {};
    bool reportMediaProgress_ {false};
    int64_t reportProgressIntervalMs_ {100}; // default interval is 100 ms
};
}  // namespace Media
}  // namespace OHOS
#endif // HISTREAMER_HIPLAYER_CALLBACKER_LOOPER_H
