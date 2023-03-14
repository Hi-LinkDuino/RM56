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

#include <chrono>
#include <assert.h>
#include <signal.h>
#include <sys/prctl.h>
#include "watchdog.h"

namespace OHOS::Camera {

WatchDog::WatchDog() {}

void WatchDog::Init(int ms, std::function<void()> executor, bool isKill)
{
    timeMs_ = ms;
    isKill_ = isKill;
    executor_ = executor;
    handleThread_ = std::make_unique<std::thread>(&WatchDog::WaitForWakeUP, this);
}

WatchDog::~WatchDog()
{
    {
        std::unique_lock<std::mutex> l(lock_);
        terminate_ = true;
        cv_.notify_one();
    }
    if (handleThread_ != nullptr && handleThread_->joinable()) {
        handleThread_->join();
    }
}

void WatchDog::WaitForWakeUP()
{
    prctl(PR_SET_NAME, "camera_watchdog");

    std::unique_lock<std::mutex> l(lock_);
    cv_.wait_for(l, std::chrono::milliseconds(timeMs_), [this]() {
        return terminate_; });
    if (terminate_) {
        return;
    }

    if (executor_) {
        executor_();
    }

    if (isKill_) {
        KillProcess();
    }
    return;
}

void WatchDog::KillProcess()
{
    raise(SIGKILL);
}

} // namespace Camera
