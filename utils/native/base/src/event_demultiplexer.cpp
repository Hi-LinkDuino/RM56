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

#include "event_demultiplexer.h"
#include "event_reactor.h"
#include "event_handler.h"
#include "common_timer_errors.h"
#include "utils_log.h"

#include <vector>
#include <cstdio>
#include <strings.h>
#include <unistd.h>
#include <sys/epoll.h>

namespace OHOS {
namespace Utils {

static const int EPOLL_MAX_EVENS_INIT = 8;
static const int HALF_OF_MAX_EVENT = 2;
static const int EPOLL_INVALID_FD = -1;

EventDemultiplexer::EventDemultiplexer()
    : epollFd_(epoll_create1(EPOLL_CLOEXEC)), maxEvents_(EPOLL_MAX_EVENS_INIT), mutex_(), eventHandlers_()
{
}

EventDemultiplexer::~EventDemultiplexer()
{
    CleanUp();
}

uint32_t EventDemultiplexer::StartUp()
{
    if (epollFd_ < 0) {
        epollFd_ = epoll_create1(EPOLL_CLOEXEC);
        if (epollFd_ < 0) {
            UTILS_LOGE("epoll_create1 failed.");
            return TIMER_ERR_BADF;
        }
    }
    return TIMER_ERR_OK;
}

void EventDemultiplexer::CleanUp()
{
    if (epollFd_ != EPOLL_INVALID_FD) {
        close(epollFd_);
        epollFd_ = EPOLL_INVALID_FD;
    }
}

uint32_t EventDemultiplexer::UpdateEventHandler(EventHandler* handler)
{
    if (handler == nullptr) {
        UTILS_LOGE("event handler is null.");
        return TIMER_ERR_INVALID_VALUE;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto itor = eventHandlers_.find(handler->GetHandle());
    if (itor == eventHandlers_.end()) {
        eventHandlers_.insert(std::make_pair(handler->GetHandle(), handler));
        return Update(EPOLL_CTL_ADD, handler);
    }

    if (handler->Events() == EventReactor::NONE_EVENT) {
        eventHandlers_.erase(itor);
        return Update(EPOLL_CTL_DEL, handler);
    }

    if (handler != itor->second) {
        UTILS_LOGE("invalid event handler!");
        return TIMER_ERR_DEAL_FAILED;
    }
    return Update(EPOLL_CTL_MOD, handler);
}

uint32_t EventDemultiplexer::RemoveEventHandler(EventHandler* handler)
{
    if (handler == nullptr) {
        UTILS_LOGE("event handler is null.");
        return TIMER_ERR_INVALID_VALUE;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto itor = eventHandlers_.find(handler->GetHandle());
    if (itor == eventHandlers_.end()) {
        return TIMER_ERR_OK;
    }

    eventHandlers_.erase(itor);
    if (static_cast<int>(eventHandlers_.size()) < maxEvents_) {
        maxEvents_ = eventHandlers_.size() / HALF_OF_MAX_EVENT;
    }

    return Update(EPOLL_CTL_DEL, handler);
}

uint32_t EventDemultiplexer::Update(int operation, EventHandler* handler)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events   = Reactor2Epoll(handler->Events());
    event.data.ptr = reinterpret_cast<void*>(handler);

    if (epoll_ctl(epollFd_, operation, handler->GetHandle(), &event) != 0) {
        UTILS_LOGE("epoll_ctl %{public}d  operation %{public}d on handle %{public}d failed",
            epollFd_, operation, handler->GetHandle());
        return TIMER_ERR_DEAL_FAILED;
    }
    return TIMER_ERR_OK;
}

void EventDemultiplexer::Polling(int timeout /* ms */)
{
    std::vector<struct epoll_event> epollEvents(maxEvents_);
    int nfds = epoll_wait(epollFd_, &epollEvents[0], static_cast<int>(epollEvents.size()), timeout);
    if (nfds == 0) {
        return;
    }
    if (nfds == -1) {
        UTILS_LOGE("epoll_wait failed.");
        return;
    }

    for (int idx = 0; idx < nfds; ++idx) {
        int events = epollEvents[idx].events;
        void* ptr = epollEvents[idx].data.ptr;
        auto handler = reinterpret_cast<EventHandler*>(ptr);
        if (handler != nullptr) {
            handler->HandleEvents(Epoll2Reactor(events));
        }
    }

    if (nfds == maxEvents_) {
        maxEvents_ *= HALF_OF_MAX_EVENT;
    }
}

uint32_t EventDemultiplexer::Epoll2Reactor(uint32_t epollEvents)
{
    if ((epollEvents & EPOLLHUP) && !(epollEvents & EPOLLIN)) {
        return EventReactor::CLOSE_EVENT;
    }

    if (epollEvents & EPOLLERR) {
        return EventReactor::ERROR_EVENT;
    }

    if (epollEvents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        return EventReactor::READ_EVENT;
    }

    if (epollEvents & EPOLLOUT) {
        return EventReactor::WRITE_EVENT;
    }

    return EventReactor::NONE_EVENT;
}

uint32_t EventDemultiplexer::Reactor2Epoll(uint32_t reactorEvent)
{
    switch (reactorEvent) {
        case EventReactor::NONE_EVENT:
            return TIMER_ERR_OK;
        case EventReactor::READ_EVENT:
            return EPOLLIN | EPOLLPRI;
        case EventReactor::WRITE_EVENT:
            return EPOLLOUT;
        case EventReactor::READ_EVENT | EventReactor::WRITE_EVENT:
            return EPOLLIN | EPOLLPRI | EPOLLOUT;
        default:
            UTILS_LOGE("invalid event %{public}u.", reactorEvent);
            return TIMER_ERR_DEAL_FAILED;
    }
}

}
}
