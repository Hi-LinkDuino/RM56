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

#ifndef UTILS_BASE_BLOCK_QUEUE_H
#define UTILS_BASE_BLOCK_QUEUE_H

#include <climits>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>

namespace OHOS {

template <typename T>
class SafeBlockQueue {
public:
    SafeBlockQueue(int capacity) : maxSize_(capacity)
    {
    }

    virtual void Push(T const& elem)
    {
        std::unique_lock<std::mutex> lock(mutexLock_);
        while (queueT_.size() >= maxSize_) {
            // queue full , waiting for jobs to be taken
            cvNotFull_.wait(lock, [&]() { return (queueT_.size() < maxSize_); });
        }

        // here means not full we can push in
        queueT_.push(elem);
        cvNotEmpty_.notify_one();
    }

    T Pop()
    {
        std::unique_lock<std::mutex> lock(mutexLock_);

        while (queueT_.empty()) {
            // queue empty, waiting for tasks to be Push
            cvNotEmpty_.wait(lock, [&] { return !queueT_.empty(); });
        }

        T elem = queueT_.front();
        queueT_.pop();
        cvNotFull_.notify_one();
        return elem;
    }

    virtual bool PushNoWait(T const& elem)
    {
        std::unique_lock<std::mutex> lock(mutexLock_);
        if (queueT_.size() >= maxSize_) {
            return false;
        }
        // here means not full we can push in
        queueT_.push(elem);
        cvNotEmpty_.notify_one();
        return true;
    }

    bool PopNotWait(T& outtask)
    {
        std::unique_lock<std::mutex> lock(mutexLock_);
        if (queueT_.empty()) {
            return false;
        }
        outtask = queueT_.front();
        queueT_.pop();

        cvNotFull_.notify_one();

        return true;
    }

    unsigned int Size()
    {
        std::unique_lock<std::mutex> lock(mutexLock_);
        return queueT_.size();
    }

    bool IsEmpty()
    {
        std::unique_lock<std::mutex> lock(mutexLock_);
        return queueT_.empty();
    }

    bool IsFull()
    {
        std::unique_lock<std::mutex> lock(mutexLock_);
        return queueT_.size() == maxSize_;
    }

    virtual ~SafeBlockQueue() {}

protected:
    unsigned long maxSize_;
    std::mutex mutexLock_;
    std::condition_variable cvNotEmpty_;
    std::condition_variable cvNotFull_;
    std::queue<T> queueT_;
};

template <typename T>
class SafeBlockQueueTracking : public SafeBlockQueue<T> {
public:
    SafeBlockQueueTracking(int capacity) : SafeBlockQueue<T>(capacity)
    {
        unfinishedTaskCount_ = 0;
    }

    virtual ~SafeBlockQueueTracking() {}

    virtual void Push(T const& elem)
    {
        unfinishedTaskCount_++;
        std::unique_lock<std::mutex> lock(mutexLock_);
        while (queueT_.size() >= maxSize_) {
            // queue full , waiting for jobs to be taken
            cvNotFull_.wait(lock, [&]() { return (queueT_.size() < maxSize_); });
        }

        // here means not full we can push in
        queueT_.push(elem);

        cvNotEmpty_.notify_one();
    }

    virtual bool PushNoWait(T const& elem)
    {
        std::unique_lock<std::mutex> lock(mutexLock_);
        if (queueT_.size() >= maxSize_) {
            return false;
        }
        // here means not full we can push in
        queueT_.push(elem);
        unfinishedTaskCount_++;
        cvNotEmpty_.notify_one();
        return true;
    }

    bool OneTaskDone()
    {
        std::unique_lock<std::mutex> lock(mutexLock_);
        int unfinished = unfinishedTaskCount_ - 1;

        if (unfinished <= 0) {
            if (unfinished < 0) {
                return false; // false mean call elem done too many times
            }
            cvAllTasksDone_.notify_all();
        }

        unfinishedTaskCount_ = unfinished;
        return true;
    }

    void Join()
    {
        std::unique_lock<std::mutex> lock(mutexLock_);
        cvAllTasksDone_.wait(lock, [&] { return unfinishedTaskCount_ == 0; });
    }

    int GetUnfinishTaskNum()
    {
        return unfinishedTaskCount_;
    }

protected:
    using SafeBlockQueue<T>::maxSize_;
    using SafeBlockQueue<T>::mutexLock_;
    using SafeBlockQueue<T>::cvNotEmpty_;
    using SafeBlockQueue<T>::cvNotFull_;
    using SafeBlockQueue<T>::queueT_;

    std::atomic<int> unfinishedTaskCount_;
    std::condition_variable cvAllTasksDone_;
};

} // namespace OHOS

#endif
