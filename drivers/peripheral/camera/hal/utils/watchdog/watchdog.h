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

#ifndef HOS_CAMERA_WATCHDOG_H
#define HOS_CAMERA_WATCHDOG_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

namespace OHOS::Camera {
class WatchDog {
public:
    WatchDog();
    void Init(int ms, std::function<void()> executor, bool isKill = false);
    ~WatchDog();

private:
    void WaitForWakeUP();
    void KillProcess();

private:
    int timeMs_ = 0;
    std::function<void()> executor_ = nullptr;
    std::condition_variable cv_;
    std::mutex lock_;
    std::unique_ptr<std::thread> handleThread_ = nullptr;
    bool terminate_ = false;
    bool isKill_ = false;
};

#define WATCHDOG_TIMEOUT 10000

#define PLACE_A_WATCHDOG(t, f, k) do { \
    WatchDog _dog;                \
    _dog.Init(t, f, k); \
} while (0)

#define PLACE_A_WATCHDOG_DEFAULT_TIME(f, k) PLACE_A_WATCHDOG(WATCHDOG_TIMEOUT, f, k)
#define PLACE_A_SELFKILL_WATCHDOG           PLACE_A_WATCHDOG_DEFAULT_TIME(nullptr, true)
#define PLACE_A_NOKILL_WATCHDOG(f)          PLACE_A_WATCHDOG_DEFAULT_TIME(f, false);
} // namespace OHOS::Camera

#endif
