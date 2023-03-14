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

#ifndef TIMER_H
#define TIMER_H

#include "base_def.h"
#include <mutex>
#include <functional>

namespace utility {
#define MS_PER_SECOND 1000
#define NS_PER_MS 1000000

class Timer {
public:
    /**
     * @brief Construct a new Timer object
     *
     * @param callback Timer's callback function.
     * @since 6
     */
    Timer(const std::function<void()> &callback);

    /**
     * @brief Destroy the Timer object
     *
     * @since 6
     */
    ~Timer();

    /**
     * @brief Start Running Timer.
     *
     * @param ms Countdown time.
     * @param isPeriodic Timer isPeriodic.
     * @return Success set timer return true, else return false.
     * @since 6
     */
    bool Start(int ms, bool isPeriodic = false);

    /**
     * @brief Stop Running Timer.
     *
     * @return Success stop timer return true, else return false.
     * @since 6
     */
    bool Stop();

private:
    int fd_ {-1};
    std::mutex mutex_ {};
    std::function<void()> callback_ {};

    friend class TimerManager;
    BT_DISALLOW_COPY_AND_ASSIGN(Timer);
};
}  // namespace utility

#endif  // TIMER_H
