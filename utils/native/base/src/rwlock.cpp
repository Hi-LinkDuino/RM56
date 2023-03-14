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

#include "rwlock.h"
#include <cassert>

namespace OHOS {
namespace Utils {

RWLock::RWLock(bool writeFirst)
    : writeFirst_(writeFirst), writeThreadID_(), lockCount_(0), writeWaitCount_(0)
{
}

void RWLock::LockRead()
{
    // If the thread has obtained the write lock, return directly.
    if (std::this_thread::get_id() == writeThreadID_) {
        return;
    }

    int count;
    if (writeFirst_) {
        do {
            // In write priority mode, the state must be non-write locked and no other threads are waiting to write
            while ((count = lockCount_) == LOCK_STATUS_WRITE || writeWaitCount_ > 0) {}
        } while (!lockCount_.compare_exchange_weak(count, count + 1));
    } else {
        do {
            // If it is not write priority, you only need the current state to be non-write-locked.
            while ((count = lockCount_) == LOCK_STATUS_WRITE) {}
        } while (!lockCount_.compare_exchange_weak(count, count + 1));
    }
}

void RWLock::UnLockRead()
{
    // Supports the case of writing and reading nesting.
    // If the write lock has been obtained before, the read lock is directly returned successfully,
    // and then the thread is still directly returned when unlocking.
    if (std::this_thread::get_id() != writeThreadID_) {
        --lockCount_;
    }
}

void RWLock::LockWrite()
{
    // If this thread is already a thread that gets the write lock, return directly to avoid repeated locks.
    if (std::this_thread::get_id() != writeThreadID_) {
        ++writeWaitCount_; // Write wait counter plus 1

        // Only when no thread has acquired a read lock or a write lock (the lock counter status is FREE)
        // can the write lock be acquired and the counter set to WRITE; otherwise wait
        for(int status = LOCK_STATUS_FREE; !lockCount_.compare_exchange_weak(status, LOCK_STATUS_WRITE);
            status = LOCK_STATUS_FREE) {
        }

        // After the write lock is successfully acquired, the write wait counter is decremented by 1.
        --writeWaitCount_;
        writeThreadID_ = std::this_thread::get_id();
    }
}

void RWLock::UnLockWrite()
{
    if (std::this_thread::get_id() != writeThreadID_) {
        return;
    }

    if (lockCount_ != LOCK_STATUS_WRITE) {
        return;
    }

    writeThreadID_ = std::thread::id();
    lockCount_.store(LOCK_STATUS_FREE);
}

} // namespace Utils
} // namespace OHOS
