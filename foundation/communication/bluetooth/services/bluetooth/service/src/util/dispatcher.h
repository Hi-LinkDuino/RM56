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

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <thread>
#include "fixed_queue.h"

namespace utility {
class Dispatcher {
public:
    /**
     * @brief Construct a new Dispatcher object.
     *
     * @param name Dispatcher name.
     * @since 6
     */
    explicit Dispatcher(const std::string &name = "bt-dispatcher");

    /**
     * @brief Destroy the Dispatcher object
     *
     * @since 6
     */
    virtual ~Dispatcher();

    /**
     * @brief Initialize the Dispatcher object
     *
     * @since 6
     */
    void Initialize();

    /**
     * @brief Uninitialize the Dispatcher object
     *
     * @since 6
     */
    void Uninitialize();

    /**
     * @brief PostTask to dispatcher.
     *
     * @param task
     * @since 6
     */
    void PostTask(const std::function<void()> &task);

    /**
     * @brief Get Dispatcher name.
     *
     * @return Dispatcher's name.
     * @since 6
     */
    const std::string &Name() const;

private:
    /**
     * @brief Run Dispatcher function.
     *
     * @param dispatcher Dispatcher pointer.
     * @since 6
     */
    void Run(std::promise<void> promise);

    std::string name_ {""};
    std::mutex mutex_ {};
    std::unique_ptr<std::thread> thread_ {nullptr};
    std::atomic_bool start_ = ATOMIC_FLAG_INIT;
    utility::FixedQueue<std::function<void()>> taskQueue_ {};

    BT_DISALLOW_COPY_AND_ASSIGN(Dispatcher);
};
}  // namespace utility

#endif  // DISPATCHER_H