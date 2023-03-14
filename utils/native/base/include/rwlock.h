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

#ifndef UTILS_RWLOCK_H
#define UTILS_RWLOCK_H

#include <atomic>
#include <thread>

#include "nocopyable.h"

namespace OHOS {
namespace Utils {

class RWLock : NoCopyable {
public:
    enum LockStatus {
        LOCK_STATUS_WRITE = -1,
        LOCK_STATUS_FREE = 0,
    };

    RWLock() : RWLock(true) {}
    explicit RWLock(bool writeFirst);
    virtual ~RWLock() {}

    void LockRead();
    void UnLockRead();

    void LockWrite();
    void UnLockWrite();

private:
    bool writeFirst_;
    std::thread::id writeThreadID_;

    // Resource lock counter, -1 is write state, 0 is free state, and greater than 0 is shared read state
    std::atomic_int lockCount_;

    // Thread counter waiting for write lock
    std::atomic_uint writeWaitCount_;
};

template <typename RWLockable>
class UniqueWriteGuard : NoCopyable {
public:
    explicit UniqueWriteGuard(RWLockable &rwLockable)
        : rwLockable_(rwLockable)
    {
        rwLockable_.LockWrite();
    }

    ~UniqueWriteGuard()
    {
        rwLockable_.UnLockWrite();
    }

private:
    UniqueWriteGuard() = delete;

private:
    RWLockable &rwLockable_;
};


template <typename RWLockable>
class UniqueReadGuard : NoCopyable {
public:
    explicit UniqueReadGuard(RWLockable &rwLockable)
        : rwLockable_(rwLockable)
    {
        rwLockable_.LockRead();
    }

    ~UniqueReadGuard()
    {
        rwLockable_.UnLockRead();
    }

private:
    UniqueReadGuard() = delete;

private:
    RWLockable &rwLockable_;
};

} // namespace Utils
} // namespace OHOS
#endif

