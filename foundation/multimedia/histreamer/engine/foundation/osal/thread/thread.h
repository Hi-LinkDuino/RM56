/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_FOUNDATION_OSAL_THREAD_H
#define HISTREAMER_FOUNDATION_OSAL_THREAD_H

#include <pthread.h> // NOLINT
#include <functional>
#include <memory> // NOLINT
#include <string>

namespace OHOS {
namespace Media {
namespace OSAL {
enum class ThreadPriority : int {
    LOW = 9,
    NORMAL = 16,
    MIDDLE = 24,
    HIGH = 32,
    HIGHEST = 39,
};

class Thread {
public:
    explicit Thread(ThreadPriority priority = ThreadPriority::HIGH) noexcept;

    Thread(const Thread&) = delete;

    Thread& operator=(const Thread&) = delete;

    Thread(Thread&& other) noexcept;

    Thread& operator=(Thread&& other) noexcept;

    virtual ~Thread() noexcept;

    bool HasThread() const noexcept;

    void SetName(const std::string& name);

    bool CreateThread(const std::function<void()>& func);

private:
    struct State {
        virtual ~State() = default;
        std::function<void()> func{};
        std::string name;
    };

    void SetNameInternal();
    static void* Run(void* arg); // NOLINT: void*

    pthread_t id_{};
    std::string name_;
    ThreadPriority priority_;
    std::unique_ptr<State> state_{};
};
} // namespace OSAL
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_OSAL_THREAD_H
