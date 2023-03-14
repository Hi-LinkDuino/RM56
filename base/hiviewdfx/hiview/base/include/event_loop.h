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
#ifndef HIVIEW_BASE_EVENT_LOOP_H
#define HIVIEW_BASE_EVENT_LOOP_H
#include <algorithm>
#include <atomic>
#include <future>
#include <map>
#include <memory>
#include <queue>
#include <utility>

#include <sys/types.h>

#include "defines.h"
#include "event.h"

#if defined(__HIVIEW_OHOS__)
#include "unique_fd.h"
#ifdef USE_POLL
#include <poll.h>
#endif
#elif defined(_WIN32)
#include <Windows.h>
#include <winnt.h>
#endif

namespace OHOS {
namespace HiviewDFX {
constexpr int LOOP_WAKEUP_HANDLE_INDEX = 0;
constexpr int MAX_EVENT_SIZE = 16;
constexpr int MAX_HANDLE_ARRAY_SIZE = 1;
constexpr int MAX_WATCHED_FDS = 64;
constexpr uint64_t SECOND_TO_MICROSECOND = 1000000;
constexpr uint64_t SECOND_TO_NANOSECOND = 1000000000;
constexpr uint64_t NANOSECOND_TO_MILLSECOND = 1000000;
constexpr uint64_t MICROSECOND_TO_MILLSECOND = 1000;
constexpr uint64_t MICROSECOND_TO_NANOSECOND = 1000;
using Task = std::function<void()>;
enum LoopEventType {
    LOOP_EVENT_TASK,
    LOOP_PACKAGED_TASK,
};
struct LoopEvent {
    bool isRepeat = false;
    uint8_t taskType = 0;
    uint64_t seq = 0;
    uint64_t interval = 0;
    uint64_t enqueueTime = 0;
    uint64_t targetTime = 0;
    std::shared_ptr<Event> event = nullptr;
    std::shared_ptr<EventHandler> handler = nullptr;
    Task task = nullptr;
    std::shared_ptr<std::packaged_task<bool()>> packagedTask = nullptr;

    static LoopEvent CreateLoopEvent(uint64_t now)
    {
        LoopEvent event;
        event.isRepeat = false;
        event.taskType = LOOP_EVENT_TASK;
        event.seq = now;
        event.interval = 0;
        event.enqueueTime = now;
        event.targetTime = now;
        event.event = nullptr;
        event.handler = nullptr;
        event.task = nullptr;
        event.packagedTask = nullptr;
        return event;
    }

    bool operator<(const LoopEvent &obj) const
    {
        // as we use std::priority_queue, the event with smaller target time will be in the top of the queue
        return (this->targetTime > obj.targetTime);
    }
};

class EventLoopPriorityQueue : public std::priority_queue<LoopEvent, std::vector<LoopEvent>> {
public:
    bool remove(uint64_t seq)
    {
        auto it = std::find_if(this->c.begin(), this->c.end(), [seq](LoopEvent event) {
            return event.seq == seq;
        });

        if (it != this->c.end()) {
            this->c.erase(it);
            std::make_heap(this->c.begin(), this->c.end(), this->comp);
            return true;
        } else {
            return false;
        };
    };
};

class FileDescriptorEventCallback {
public:
    virtual ~FileDescriptorEventCallback(){};
#if defined(__HIVIEW_OHOS__)
    virtual bool OnFileDescriptorEvent(int fd, int Type) = 0;
    virtual int32_t GetPollFd() = 0;
    virtual int32_t GetPollType() = 0;
#elif defined(_WIN32)
    virtual bool OnHandleEvent(std::string fileName, DWORD action) = 0;
#endif
};
class DllExport EventLoop {
public:
    explicit EventLoop(const std::string &name);
    virtual ~EventLoop();
    void StartLoop(bool createNewThread = true);
    void StopLoop();

    // poll event from file descriptor source
    // the interfaces may change on windows platform
    bool AddFileDescriptorEventCallback(const std::string &name, std::shared_ptr<FileDescriptorEventCallback> source);
    bool RemoveFileDescriptorEventCallback(const std::string &name);

    // process event immediately
    uint64_t AddEvent(std::shared_ptr<EventHandler> handler, std::shared_ptr<Event> event, const Task task = nullptr);
    std::future<bool> AddEventForResult(std::shared_ptr<EventHandler> handler, std::shared_ptr<Event> event);

    // process delayed event
    // interval in seconds
    uint64_t AddTimerEvent(std::shared_ptr<EventHandler> handler, std::shared_ptr<Event> event, const Task &task,
        uint64_t interval, bool repeat);
    bool RemoveEvent(uint64_t seq);

    const std::string &GetName() const
    {
        return name_;
    };

    bool IsRunning() const
    {
        return isRunning_;
    };

private:
    // call from audit module
    std::string GetHandlerInfo(const LoopEvent &)
    {
        return "";
    };

    bool InitEventQueueNotifier();
    void Run();
    void WakeUp();
    uint64_t ProcessQueuedEvent();
    void WaitNextEvent(uint64_t leftTimeMill);
    bool FetchNextEvent(uint64_t now, uint64_t& leftTimeNanosecond, LoopEvent& event);
    void ProcessEvent(LoopEvent &event);
    void ReInsertPeriodicEvent(uint64_t now, LoopEvent &event);
    void ResetTimerIfNeedLocked();
    uint64_t NanoSecondSinceSystemStart();
    volatile bool isWaken_ = false;
    volatile bool needQuit_ = false;
    volatile bool isRunning_ = false;
    std::string name_;
    EventLoopPriorityQueue pendingEvents_;
    std::unique_ptr<std::thread> thread_;
    std::mutex queueMutex_;
#if defined(__HIVIEW_OHOS__)
#ifdef USE_POLL
    void ModifyFdStatus();
    void PollNextEvent(uint64_t timeout);
    volatile bool modifyFdStatus_ = false;
    int32_t eventQueueFd_[2] = {-1, -1}; // 2:event queue fd size
    int32_t watchedFdSize_ = 1;
    struct pollfd watchFds_[MAX_WATCHED_FDS];
#else
    UniqueFd pendingEventQueueFd_;
    UniqueFd sharedPollingFd_;
#endif
    std::map<int32_t, std::shared_ptr<FileDescriptorEventCallback>> eventSourceMap_;
    std::map<std::string, int32_t> eventSourceNameMap_;
#elif defined(_WIN32)
    HANDLE watchHandleList_[MAX_HANDLE_ARRAY_SIZE] = {NULL};
#endif
    uint64_t nextWakeupTime_;
    std::atomic<LoopEvent *> currentProcessingEvent_;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEW_BASE_EVENT_LOOP_H