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

#include "timer.h"

#include <algorithm>
#include "common_timer_errors.h"
#include <atomic>
#include <sys/prctl.h>
#include "timer_event_handler.h" /* for INVALID_TIMER_FD */
#include "utils_log.h"
namespace OHOS {
namespace Utils {

Timer::Timer(const std::string& name, int timeoutMs) : name_(name), timeoutMs_(timeoutMs),
    reactor_(new EventReactor())
{
}

uint32_t Timer::Setup()
{
    std::thread loop_thread(std::bind(&Timer::MainLoop, this));
    thread_.swap(loop_thread);

    return TIMER_ERR_OK;
}

void Timer::Shutdown(bool useJoin)
{
    if (reactor_->IsStopped()) {
        UTILS_LOGD("timer has been stopped already");
        return;
    }

    reactor_->StopLoop();
    if (timeoutMs_ == -1) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (intervalToTimers_.empty()) {
            UTILS_LOGI("no event for epoll wait, use detach to shutdown");
            thread_.detach();
            return;
        }
    }
    if (!useJoin) {
        thread_.detach();
        return;
    }
    thread_.join();
}

uint32_t Timer::Register(const TimerCallback& callback, uint32_t interval /* ms */, bool once)
{
    std::lock_guard<std::mutex> lock(mutex_);
    static std::atomic_uint32_t timerId = 1;
    int timerFd = once ? INVALID_TIMER_FD : GetTimerFd(interval);
    if (timerFd == INVALID_TIMER_FD) {
        uint32_t ret = DoRegister(std::bind(&Timer::OnTimer, this, std::placeholders::_1), interval, once, timerFd);
        if (ret != TIMER_ERR_OK) {
            UTILS_LOGE("do register interval timer %{public}d failed, return %{public}u", interval, ret);
            return TIMER_ERR_DEAL_FAILED;
        }
    }

    timerId = GetValidId(timerId);
    while (timerToEntries_.find(timerId) != timerToEntries_.end()) {
        timerId++;
        timerId = GetValidId(timerId);
    }

    TimerEntryPtr entry(new TimerEntry());
    entry->timerId = timerId++;
    entry->interval = interval;
    entry->callback = callback;
    entry->once = once;
    entry->timerFd = timerFd;

    intervalToTimers_[interval].push_back(entry);
    timerToEntries_[entry->timerId] = entry;

    UTILS_LOGD("register timer %{public}u with %{public}u ms interval.", entry->timerId, entry->interval);
    return entry->timerId;
}

void Timer::Unregister(uint32_t timerId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (timerToEntries_.find(timerId) == timerToEntries_.end()) {
        UTILS_LOGD("timer %{public}u does not exist", timerId);
        return;
    }

    auto entry = timerToEntries_[timerId];
    UTILS_LOGD("deregister timer %{public}u with %{public}u ms interval", timerId, entry->interval);

    auto itor = intervalToTimers_[entry->interval].begin();
    for (; itor != intervalToTimers_[entry->interval].end(); ++itor) {
        if ((*itor)->timerId == timerId) {
            UTILS_LOGD("erase timer %{public}u.", timerId);
            if ((*itor)->once) {
                reactor_->CancelTimer((*itor)->timerFd);
                timers_.erase((*itor)->timerFd);
            }
            intervalToTimers_[entry->interval].erase(itor);
            break;
        }
    }

    if (intervalToTimers_[entry->interval].empty()) {
        UTILS_LOGD("deregister timer interval: %{public}u.", entry->interval);
        intervalToTimers_.erase(entry->interval);
        DoUnregister(entry->interval);
    }
    timerToEntries_.erase(timerId);
}

void Timer::MainLoop()
{
    prctl(PR_SET_NAME, name_.c_str(), 0, 0, 0);
    if (reactor_->StartUp() == TIMER_ERR_OK) {
        reactor_->RunLoop(timeoutMs_);
    }
    reactor_->CleanUp();
}

uint32_t Timer::DoRegister(const TimerListCallback& callback, uint32_t interval, bool once, int &timerFd)
{
    using namespace std::placeholders;
    std::function<void(int)> cb = std::bind(&Timer::DoTimerListCallback, this, callback, _1);
    uint32_t ret = reactor_->ScheduleTimer(cb, interval, timerFd, once);
    if ((ret != TIMER_ERR_OK) || (timerFd < 0)) {
        UTILS_LOGE("ScheduleTimer failed!ret:%{public}d, timerFd:%{public}d", ret, timerFd);
        return ret;
    }
    timers_[timerFd] = interval;
    return TIMER_ERR_OK;
}

void Timer::DoUnregister(uint32_t interval)
{
    for (auto& itor : timers_) {
        if (itor.second == interval) {
            reactor_->CancelTimer(itor.first);
        }
    }
}

void Timer::OnTimer(int timerFd)
{
    uint32_t interval = timers_[timerFd];
    TimerEntryList entryList;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        entryList = intervalToTimers_[interval];
    }

    std::vector<uint32_t> onceIdsUnused;
    for (const TimerEntryPtr& ptr : entryList) {
        if (ptr->timerFd != timerFd) {
            continue;
        }
        /* if stop, callback is forbidden */
        if (!reactor_->IsStopped()) {
            ptr->callback();
        }

        if (!ptr->once) {
            continue;
        }
        onceIdsUnused.push_back(ptr->timerId);
    }

    if (!onceIdsUnused.empty()) {
        EraseUnusedTimerId(interval, onceIdsUnused);
    }
}

void Timer::DoTimerListCallback(const TimerListCallback& callback, int timerFd)
{
    callback(timerFd);
}

/* valid range: [1, UINT32_MAX], but not TIMER_ERR_DEAL_FAILED */
uint32_t Timer::GetValidId(uint32_t timerId) const
{
    if (timerId == TIMER_ERR_DEAL_FAILED) {
        return timerId + 1;
    }
    if (timerId == UINT32_MAX) {
        return 1;
    }
    return timerId;
}

int Timer::GetTimerFd(uint32_t interval /* ms */)
{
    if (intervalToTimers_.find(interval) == intervalToTimers_.end()) {
        return INVALID_TIMER_FD;
    }
    auto &entryList = intervalToTimers_[interval];
    for (const TimerEntryPtr &ptr : entryList) {
        if (!ptr->once) {
            return ptr->timerFd;
        }
    }
    return INVALID_TIMER_FD;
}

void Timer::EraseUnusedTimerId(uint32_t interval, const std::vector<uint32_t>& unusedIds)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto &entryList = intervalToTimers_[interval];
    for (auto itor = entryList.begin(); itor != entryList.end();) {
        uint32_t id = (*itor)->timerId;
        if (std::find(unusedIds.begin(), unusedIds.end(), id) == unusedIds.end()) {
            ++itor;
            continue;
        }

        reactor_->CancelTimer((*itor)->timerFd);
        timers_.erase((*itor)->timerFd);
        itor = entryList.erase(itor);
        timerToEntries_.erase(id);

        if (entryList.empty()) {
            intervalToTimers_.erase(interval);
            DoUnregister(interval);
            return;
        }
    }
}

} // namespace Utils
} // namespace OHOS
