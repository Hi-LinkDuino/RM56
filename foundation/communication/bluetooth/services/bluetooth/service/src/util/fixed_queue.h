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

#ifndef FIXED_QUEUE_H
#define FIXED_QUEUE_H

#include <memory>
#include <mutex>
#include <queue>
#include "semaphore_utils.h"

namespace utility {
template<class T>
class FixedQueue {
public:
    /**
     * @brief Construct a new Fixed Queue object
     *
     * @param capacity Queue's capacity, default is 20.
     * @since 6
     */
    explicit FixedQueue(const size_t capacity = 20);

    /**
     * @brief Destroy the Fixed Queue object
     *
     * @since 6
     */
    virtual ~FixedQueue();

    /**
     * @brief Try push one record into FixedQueue.
     *
     * @param record Push record.
     * @return Success push record return true, else return false.
     * @since 6
     */
    bool TryPush(T record);

    /**
     * @brief Push one record into FixedQueue.
     *
     * @param record Push record.
     * @since 6
     */
    void Push(T record);

    /**
     * @brief Try pop one record from FixedQueue.
     *
     * @param record Pop record object result.
     * @return Success pop record return true, else return false.
     * @since 6
     */
    bool TryPop(T &record);

    /**
     * @brief Pop one record from FixedQueue.
     *
     * @param record Pop record object result.
     * @since 6
     */
    void Pop(T &record);

private:
    std::queue<T> queue_ {};
    std::mutex mutex_ {};
    std::unique_ptr<Semaphore> enqueue_ {};
    std::unique_ptr<Semaphore> dequeue_ {};
    size_t capacity_ {20};

    BT_DISALLOW_COPY_AND_ASSIGN(FixedQueue);
};

template<class T>
FixedQueue<T>::FixedQueue(const size_t capacity) : capacity_(capacity)
{
    enqueue_ = std::make_unique<Semaphore>(capacity_);
    dequeue_ = std::make_unique<Semaphore>(0);
}

template<class T>
FixedQueue<T>::~FixedQueue()
{
    while (!queue_.empty()) {
        queue_.pop();
    }
}

template<class T>
bool FixedQueue<T>::TryPush(T record)
{
    if (!enqueue_->TryWait()) {
        return false;
    } else {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(record));
        }
        dequeue_->Post();
        return true;
    }
}

template<class T>
void FixedQueue<T>::Push(T record)
{
    enqueue_->Wait();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(record));
    }
    dequeue_->Post();
}

template<class T>
bool FixedQueue<T>::TryPop(T &record)
{
    if (!dequeue_->TryWait()) {
        return false;
    } else {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            record = std::move(queue_.front());
            queue_.pop();
        }
        enqueue_->Post();
        return true;
    }
}

template<class T>
void FixedQueue<T>::Pop(T &record)
{
    dequeue_->Wait();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        record = std::move(queue_.front());
        queue_.pop();
    }
    enqueue_->Post();
}
}  // namespace utility

#endif  // FIXED_QUEUE_H