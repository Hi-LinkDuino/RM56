/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef SEMAPHORE_UTILS_H
#define SEMAPHORE_UTILS_H

#include <condition_variable>
#include <mutex>
#include "base_def.h"

namespace utility {
class Semaphore {
public:
    /**
     * @brief Construct a new Semaphore object.
     *
     * @param val Initial value of the Semaphore.
     * @since 6
     */
    explicit Semaphore(int val = 1) : count_(val){};

    /**
     * @brief Destroy the Semaphore object.
     *
     * @since 6
     */
    ~Semaphore() = default;

    /**
     * @brief Semaphore val reduce 1.
     *        If val after reduce operation is less than 0,
     *        block waiting until other call Post() to increase Semaphore val greater or equal to 0.
     *
     * @since 6
     */
    void Wait();

    /**
     * @brief Semaphore val try reduce 1.
     *        If val after reduce operation is less than 0,
     *        discard execution, return false.
     *
     * @return Success reduce Semaphore val return true, else return false.
     * @since 6
     */
    bool TryWait();

    /**
     * @brief Semaphore val increase 1.
     *
     * @since 6
     */
    void Post();

private:
    int count_ {1};
    int wakeupnum_ {0};
    std::mutex mutex_ {};
    std::condition_variable condVar_ {};

    BT_DISALLOW_COPY_AND_ASSIGN(Semaphore);
};
}  // namespace utility

#endif  // SEMAPHORE_UTILS_H