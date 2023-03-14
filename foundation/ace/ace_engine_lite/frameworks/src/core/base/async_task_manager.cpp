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

#include "async_task_manager.h"
#include "ace_log.h"
#include "fatal_handler.h"

#if (defined(__LINUX__) || defined(__LITEOS_A__))
#define TRY_LOCK()                  \
    {                               \
        pthread_mutex_lock(&lock_); \
    }

#define TRY_UNLOCK()                  \
    {                                 \
        pthread_mutex_unlock(&lock_); \
    }
#elif defined(__LITEOS_M__)
#define TRY_LOCK()      \
    {                   \
        LOS_TaskLock(); \
    }
#define TRY_UNLOCK()      \
    {                     \
        LOS_TaskUnlock(); \
    }
#else
#define TRY_LOCK() \
    {              \
    }
#define TRY_UNLOCK() \
    {                \
    }
#endif

namespace OHOS {
namespace ACELite {
AsyncTaskManager::AsyncTaskManager()
    : head_(nullptr),
      tail_(nullptr),
#if (defined(__LINUX__) || defined(__LITEOS_A__))
      lock_(PTHREAD_MUTEX_INITIALIZER),
#endif
      uniqueTaskID_(0),
      front_(false),
      initialized_(false)
{
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    pthread_mutex_init(&lock_, nullptr);
#endif
}

void AsyncTaskManager::Reset()
{
    while (head_ != nullptr) {
        AsyncTask *task = head_;
        head_ = head_->next;
        delete task;
        task = nullptr;
    }
    tail_ = nullptr;
}

AsyncTaskManager &AsyncTaskManager::GetInstance()
{
    static AsyncTaskManager instance;
    return instance;
}

void AsyncTaskManager::Init()
{
    Reset(); // make sure no residual task in list
    if (initialized_) {
        // do not add repeatly
        return;
    }
    Task::Init();
    initialized_ = true;
}

void AsyncTaskManager::Callback()
{
    if (!front_) {
        return;
    }

    while (head_ != nullptr) {
        AsyncTask *task = head_;
        task->isRunning = true;
        task->handler(task->data);
        if (head_ == tail_) {
            tail_ = nullptr;
        }
        head_ = head_->next;
        delete task;
        task = nullptr;
    }
}

uint16_t AsyncTaskManager::Dispatch(AsyncTaskHandler handler, void *data, const void *context)
{
    if (handler == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AsyncTaskManager::Dispatch failed: handler is null.");
        return DISPATCH_FAILURE;
    }
    if (FatalHandler::GetInstance().IsFatalErrorHitted()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AsyncTaskManager::Dispatch failed: Fatal error is hitted.");
        return DISPATCH_FAILURE;
    }
    TRY_LOCK();
    auto *task = new AsyncTask();
    if (task == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AsyncTaskManager::Dispatch failed: out of memory.");
        TRY_UNLOCK();
        return DISPATCH_FAILURE;
    }
    task->handler = handler;
    task->data = data;
    task->id = (++uniqueTaskID_);
    task->context = context;
    task->isRunning = false;
    task->next = nullptr;
    if (head_ == nullptr) {
        head_ = task;
        tail_ = task;
    } else {
        tail_->next = task;
        tail_ = task;
    }
    TRY_UNLOCK();
    return uniqueTaskID_;
}

void AsyncTaskManager::Cancel(uint16_t taskID)
{
    if (DISPATCH_FAILURE == taskID) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AsyncTaskManager::Cancel failed: invalid task ID.");
        return;
    }
    TRY_LOCK();
    AsyncTask *node = head_;
    AsyncTask *prev = nullptr;
    while (node != nullptr) {
        if (node->id == taskID && !(node->isRunning)) {
            if (prev == nullptr) {
                head_ = head_->next;
            } else {
                prev->next = node->next;
                if (node == tail_) {
                    tail_ = prev;
                }
            }
            delete node;
            node = nullptr;
            break;
        }
        prev = node;
        node = node->next;
    }
    TRY_UNLOCK();
}

void AsyncTaskManager::CancelWithContext(const void *context)
{
    if (context == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AsyncTaskManager::CancelWithContext failed: null context.");
        return;
    }
    TRY_LOCK();
    AsyncTask *node = head_;
    AsyncTask *prev = nullptr;
    AsyncTask *next = nullptr;
    while (node != nullptr) {
        next = node->next;
        if ((node->context == context) && !(node->isRunning)) {
            if (prev == nullptr) {
                head_ = head_->next;
            } else {
                prev->next = next;
            }
            if (node == tail_) {
                tail_ = prev;
            }
            delete node;
            node = next;
            continue;
        }
        prev = node;
        node = next;
    }
    TRY_UNLOCK();
}

void AsyncTaskManager::SetFront(bool front)
{
    front_ = front;
}
} // namespace ACELite
} // namespace OHOS
