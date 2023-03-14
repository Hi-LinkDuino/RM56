/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <cstring>
#include <future>
#include <list>
#include <memory>
#include <thread>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include "log.h"
#include "securec.h"

namespace utility {
class TimerManager {
public:
    static TimerManager &GetInstance();

    bool AddTimer(Timer &timer);
    void RemoveTimer(Timer &timer);

private:
    static const int MAXEPOLLEVENTS = 128;
    TimerManager();
    ~TimerManager();

    void Initialize(std::promise<int> startPromise);
    void OnTimer(std::promise<int> startPromise);
    void OnCallback(Timer &timer) const;

    int epollFd_ {-1};
    int stopFd_ {-1};
    std::mutex mutex_ {};
    std::list<Timer *> unregisteredList_ {};
    std::unique_ptr<std::thread> thread_ {};
};

TimerManager &TimerManager::GetInstance()
{
    static TimerManager instance;
    return instance;
}

TimerManager::TimerManager()
{
    std::promise<int> startPromise;
    std::future<int> startFuture = startPromise.get_future();
    thread_ = std::make_unique<std::thread>(&TimerManager::OnTimer, this, std::move(startPromise));
    startFuture.wait();
}

TimerManager::~TimerManager()
{
    eventfd_write(stopFd_, 1);
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }

    if (epollFd_ != -1) {
        close(epollFd_);
    }
    if (stopFd_ != -1) {
        close(stopFd_);
    }
}

void TimerManager::Initialize(std::promise<int> startPromise)
{
    epollFd_ = epoll_create1(EPOLL_CLOEXEC);
    stopFd_ = eventfd(0, 0);
    if ((epollFd_ == -1) || (stopFd_ == -1)) {
        LOG_ERROR("TimerManager: Create epoll failed!!");
        startPromise.set_value(-1);
        return;
    }

    int ret;
    struct epoll_event event = {};

    event.data.ptr = nullptr;
    event.events = EPOLLIN;
    CHECK_EXCEPT_INTR(ret = epoll_ctl(epollFd_, EPOLL_CTL_ADD, stopFd_, &event));
    if (ret == -1) {
        LOG_ERROR("TimerManager: Epoll add event failed!!");
        startPromise.set_value(-1);
        return;
    }

    startPromise.set_value(0);
}

void TimerManager::OnCallback(Timer &timer) const
{
    uint64_t num = 0;
    int ret = read(timer.fd_, &num, sizeof(uint64_t));
    if (ret == sizeof(uint64_t)) {
        if (num > 1) {
            LOG_WARN("Timer has expired more than one time.");
        }
        timer.callback_();
    } else if (errno == EAGAIN) {
        LOG_INFO("Timer is stopped or reset before callback called.");
    } else {
        LOG_ERROR("Unknown error type.");
    }
}

void TimerManager::OnTimer(std::promise<int> startPromise)
{
    Initialize(std::move(startPromise));

    struct epoll_event events[MAXEPOLLEVENTS];
    for (;;) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            unregisteredList_.clear();
        }

        int nfds;
        CHECK_EXCEPT_INTR(nfds = epoll_wait(epollFd_, events, MAXEPOLLEVENTS, -1));
        if (nfds == -1) {
            return;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.ptr == nullptr) {
                eventfd_t val;
                eventfd_read(this->stopFd_, &val);
                return;
            }
            Timer *timer = (Timer *)events[i].data.ptr;

            std::unique_lock<std::mutex> lock(mutex_);
            auto it = std::find(unregisteredList_.begin(), unregisteredList_.end(), timer);
            if (it != unregisteredList_.end()) {
                continue;
            }
            std::lock_guard<std::mutex> itemLock(timer->mutex_);
            lock.unlock();
            OnCallback(*timer);
        }
    }
}

bool TimerManager::AddTimer(Timer &timer)
{
    struct epoll_event event = {};
    event.data.ptr = &timer;
    event.events = EPOLLIN | EPOLLRDHUP;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find(unregisteredList_.begin(), unregisteredList_.end(), &timer);
        if (it != unregisteredList_.end()) {
            unregisteredList_.remove(&timer);
        }
    }

    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, timer.fd_, &event) == -1) {
        LOG_ERROR("TimerManager add timer failed");
        return false;
    }

    return true;
}

void TimerManager::RemoveTimer(Timer &timer)
{
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, timer.fd_, nullptr) == -1) {
        LOG_ERROR("TimerManager remove timer failed");
        return;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        unregisteredList_.push_back(&timer);
    }

    timer.mutex_.lock();
    timer.mutex_.unlock();
}

Timer::Timer(const std::function<void()> &callback)
{
    fd_ = timerfd_create(CLOCK_MONOTONIC, EFD_NONBLOCK);
    if (fd_ == -1) {
        LOG_ERROR("timerfd_create ERROR");
    }
    callback_ = std::move(callback);
    if (!TimerManager::GetInstance().AddTimer(*this)) {
        LOG_ERROR("TimerManager addtimer false");
    }
}

Timer::~Timer()
{
    TimerManager::GetInstance().RemoveTimer(*this);
    if (close(fd_) == -1) {
        LOG_ERROR("TimerStop close fd failed");
    }
}

bool Timer::Start(int ms, bool isPeriodic)
{
    std::lock_guard<std::mutex> lock(mutex_);
    struct itimerspec its;
    (void)memset_s(&its, sizeof(its), 0, sizeof(its));

    its.it_value.tv_sec = ms / MS_PER_SECOND;
    its.it_value.tv_nsec = (ms % MS_PER_SECOND) * NS_PER_MS;
    if (isPeriodic) {
        its.it_interval = its.it_value;
    }

    if (timerfd_settime(fd_, 0, &its, NULL) == -1) {
        return false;
    }
    return true;
}

bool Timer::Stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    struct itimerspec its;
    (void)memset_s(&its, sizeof(its), 0, sizeof(its));

    if (timerfd_settime(fd_, 0, &its, NULL) == -1) {
        return false;
    }
    return true;
}
}  // namespace utility