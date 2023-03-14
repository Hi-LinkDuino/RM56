/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BT_HAL_HCI_WATCHER_H
#define BT_HAL_HCI_WATCHER_H

#include <atomic>
#include <ctime>
#include <functional>
#include <map>
#include <mutex>
#include <sys/time.h>
#include <thread>

namespace OHOS {
namespace HDI {
namespace BT {
namespace V1_0 {
class HciWatcher {
public:
    using HciDataCallback = std::function<void(int fd)>;
    using TimeoutCallback = std::function<void()>;

    HciWatcher();
    ~HciWatcher();

    bool AddFdToWatcher(int fd, HciDataCallback callback);
    bool RemoveFdToWatcher(int fd);
    bool SetTimeout(std::chrono::milliseconds timeout, TimeoutCallback callback);
    bool Start();
    bool Stop();

private:
    void WatcherThread();
    void ThreadWakeup();

private:
    std::atomic_bool running_ = {false};
    std::map<int, HciDataCallback> fds_;
    std::mutex fdsMutex_;
    int wakeupPipe_[2] = {0};
    timeval timeoutTimer_ = {};
    TimeoutCallback timeoutCallback_;
    std::mutex timeoutMutex_;
    std::thread thread_;
};
}  // namespace V1_0
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS
#endif /* BT_HAL_WATCHER_H */