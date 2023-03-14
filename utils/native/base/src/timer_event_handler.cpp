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

#include "timer_event_handler.h"
#include "event_reactor.h"
#include "event_handler.h"
#include "common_timer_errors.h"
#include "utils_log.h"

#include <unistd.h>
#include <sys/timerfd.h>

namespace OHOS {
namespace Utils {

// Unit of measure conversion
static const int MILLI_TO_BASE = 1000;
static const int NANO_TO_BASE = 1000000000;
constexpr int MILLI_TO_NANO = NANO_TO_BASE / MILLI_TO_BASE;

TimerEventHandler::TimerEventHandler(EventReactor* p, uint32_t timeout /* ms */, bool once)
    : once_(once),
      timerFd_(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
      interval_(timeout),
      reactor_(p),
      handler_(new EventHandler(timerFd_, p)),
      callback_()
{
}

TimerEventHandler::~TimerEventHandler()
{
    close(timerFd_);
    timerFd_ = INVALID_TIMER_FD;
}

uint32_t TimerEventHandler::Initialize()
{
    if ((timerFd_ == INVALID_TIMER_FD) || (reactor_ == nullptr) || (handler_ == nullptr)) {
        UTILS_LOGE("TimerEventHandler::initialize failed.");
        return TIMER_ERR_INVALID_VALUE;
    }

    struct itimerspec newValue = {{0, 0}, {0, 0}};
    timespec now{0, 0};
    if (clock_gettime(CLOCK_MONOTONIC, &now) == -1) {
        UTILS_LOGE("Failed clock_gettime.");
        return TIMER_ERR_DEAL_FAILED;
    }

    // next time out time is now + interval
    newValue.it_value.tv_sec = now.tv_sec + interval_ / MILLI_TO_BASE;
    newValue.it_value.tv_nsec = now.tv_nsec + (interval_ % MILLI_TO_BASE) * MILLI_TO_NANO;
    if (newValue.it_value.tv_nsec >= NANO_TO_BASE) {
        newValue.it_value.tv_sec += 1;
        newValue.it_value.tv_nsec = newValue.it_value.tv_nsec % NANO_TO_BASE;
    }

    if (once_) {
        // interval, 0 means time out only once
        newValue.it_interval.tv_sec  = 0;
        newValue.it_interval.tv_nsec = 0;
    } else {
        // interval
        newValue.it_interval.tv_sec  = interval_ / MILLI_TO_BASE;
        newValue.it_interval.tv_nsec = (interval_ % MILLI_TO_BASE) * MILLI_TO_NANO;
    }

    if (timerfd_settime(timerFd_, TFD_TIMER_ABSTIME, &newValue, nullptr) == -1) {
        UTILS_LOGE("Failed in timerFd_settime");
        return TIMER_ERR_DEAL_FAILED;
    }

    handler_->SetReadCallback(std::bind(&TimerEventHandler::TimeOut, this));
    handler_->EnableRead();
    return TIMER_ERR_OK;
}

void TimerEventHandler::Uninitialize()
{
    if (handler_ != nullptr) {
        handler_->DisableAll();
    }
}

void TimerEventHandler::TimeOut()
{
    if (timerFd_ == INVALID_TIMER_FD) {
        UTILS_LOGE("timerFd_ is invalid.");
        return;
    }
    uint64_t expirations = 0;
    ssize_t n = ::read(timerFd_, &expirations, sizeof(expirations));
    if (n != sizeof(expirations)) {
        UTILS_LOGE("epoll_loop::on_timer() reads %{public}d bytes instead of 8.", static_cast<int>(n));
    }
    if (callback_) {
        callback_(timerFd_);
    }
}

} // namespace Utils
} // namespace OHOS
