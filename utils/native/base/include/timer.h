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

#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include <sys/types.h>
#include <cstdint>
#include <string>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "../src/event_reactor.h"

namespace OHOS {
namespace Utils {

class Timer {
public:
    using TimerCallback = std::function<void ()>;
    using TimerCallbackPtr = std::shared_ptr<TimerCallback>;
    using TimerListCallback = std::function<void (int timerFd)>;

public:
    /*
     * if performance-sensitive, change "timeout" larger before Setup
     * default-value(1000ms), performance-estimate: occupy fixed-100us in every default-value(1000ms)
     * timeout: range [-1, INT32MAX], but [-1,0] is not recommended
     *          -1: wait for ever(until event-trigger);
     *          0: no wait, occupy too much cpu time;
     *          others: wait(until event-trigger)
     */
    explicit Timer(const std::string& name, int timeoutMs = 1000);
    virtual ~Timer() {}

    virtual uint32_t Setup();

    /*
     * useJoin true:    use std::thread::join(default)
     *         false:   use std::thread::detach(not recommended)
     * if timeoutMs = -1 and no valid event-trigger in epoll_wait:
     *                  use std::thread::detach inside to avoid deadloop
     */
    virtual void Shutdown(bool useJoin = true);

    uint32_t Register(const TimerCallback& callback, uint32_t interval /* ms */, bool once = false);
    void Unregister(uint32_t timerId);

private:
    void MainLoop();
    void OnTimer(int timerFd);
    virtual uint32_t DoRegister(const TimerListCallback& callback, uint32_t interval, bool once, int &timerFd);
    virtual void DoUnregister(uint32_t interval);
    void DoTimerListCallback(const TimerListCallback& callback, int timerFd);
    uint32_t GetValidId(uint32_t timerId) const;
    int GetTimerFd(uint32_t interval /* ms */);
    void EraseUnusedTimerId(uint32_t interval, const std::vector<uint32_t>& unusedIds);

private:
    struct TimerEntry {
        uint32_t       timerId;  // unique id
        uint32_t       interval;  // million second
        TimerCallback  callback;
        bool           once;
        int            timerFd;
    };

    using TimerEntryPtr = std::shared_ptr<TimerEntry>;
    using TimerEntryList = std::list<TimerEntryPtr>;

    std::map<uint32_t, TimerEntryList> intervalToTimers_;  // interval to TimerEntryList
    std::map<uint32_t, TimerEntryPtr> timerToEntries_;  // timer_id to TimerEntry

    std::string name_;
    int timeoutMs_;
    std::thread thread_;
    std::unique_ptr<EventReactor> reactor_;
    std::map<uint32_t, uint32_t> timers_;  // timer_fd to interval
    std::mutex mutex_;
};

} // namespace Utils
} // namespace OHOS
#endif

