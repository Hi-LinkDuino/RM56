/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef UTILS_EVENT_REACTOR_H
#define UTILS_EVENT_REACTOR_H

#include <sys/types.h>
#include <cstdint>
#include <memory>
#include <mutex>
#include <list>

namespace OHOS {
namespace Utils {

class EventDemultiplexer;
class EventHandler;
class TimerEventHandler;

class EventReactor {
public:
    using TimerCallback = std::function<void(int timerFd)>;
    static const uint32_t NONE_EVENT  = 0x0000;
    static const uint32_t READ_EVENT  = 0x0001;
    static const uint32_t WRITE_EVENT = 0x0002;
    static const uint32_t CLOSE_EVENT = 0x0004;
    static const uint32_t ERROR_EVENT = 0x0008;

    EventReactor();
    EventReactor(const EventReactor&) = delete;
    EventReactor& operator=(const EventReactor&) = delete;
    EventReactor(const EventReactor&&) = delete;
    EventReactor& operator=(const EventReactor&&) = delete;
    virtual ~EventReactor();

    uint32_t StartUp();
    void CleanUp();

    void RunLoop(int timeout) const;
    void StopLoop();
    bool IsStopped() const { return stopped_; }

    void UpdateEventHandler(EventHandler* handler);
    void RemoveEventHandler(EventHandler* handler);

    uint32_t ScheduleTimer(const TimerCallback& cb, uint32_t interval /* ms */, int& timerFd, bool once);
    void CancelTimer(int timerFd);

private:
    volatile bool stopped_;
    std::unique_ptr<EventDemultiplexer> demultiplexer_;
    std::recursive_mutex mutex_;
    std::list<std::shared_ptr<TimerEventHandler>> timerEventHandlers_;
};

} // namespace Utils
} // namespace OHOS
#endif
