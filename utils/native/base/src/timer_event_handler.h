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
#ifndef UTILS_TIMER_EVENT_HANDLER_H
#define UTILS_TIMER_EVENT_HANDLER_H

#include <functional>
#include <memory>
#include <cstdint>

namespace OHOS {
namespace Utils {
constexpr int INVALID_TIMER_FD = -1;

class EventHandler;
class EventReactor;

class TimerEventHandler {
    using TimerCallback = std::function<void(int timerFd)>;
    
public:
    TimerEventHandler(EventReactor* p, uint32_t timeout, bool once);
    ~TimerEventHandler();

    TimerEventHandler(const TimerEventHandler&) = delete;
    TimerEventHandler& operator=(const TimerEventHandler&) = delete;
    TimerEventHandler(const TimerEventHandler&&) = delete;
    TimerEventHandler& operator=(const TimerEventHandler&&) = delete;

    uint32_t Initialize();
    void Uninitialize();

    void SetTimerCallback(const TimerCallback& callback) { callback_ = callback; }

    uint32_t GetInterval() const { return interval_; }
    int GetTimerFd() const { return timerFd_; }

private:
    void TimeOut();

private:
    bool           once_;
    int            timerFd_;
    uint32_t       interval_;
    EventReactor*  reactor_;

    std::unique_ptr<EventHandler> handler_;
    TimerCallback                 callback_;
};

} // namespace Utils
} // namespace OHOS
#endif
