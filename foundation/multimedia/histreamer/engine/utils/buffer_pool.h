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

#ifndef HISTREAMER_FOUNDATION_BUFFER_POOL_H
#define HISTREAMER_FOUNDATION_BUFFER_POOL_H

#include <atomic>
#include <list>
#include <memory>
#include "constants.h"
#include "foundation/osal/thread/condition_variable.h"
#include "foundation/osal/thread/mutex.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "plugin/common/plugin_buffer.h"

namespace OHOS {
namespace Media {
template <typename T>
class BufferPool : public std::enable_shared_from_this<BufferPool<T>> {
public:
    explicit BufferPool<T>(size_t poolSize) : poolSize_(poolSize), isActive_(true), allocInProgress(false)
    {
    }

    BufferPool<T>(const BufferPool<T>&) = delete;

    BufferPool<T>& operator=(const BufferPool<T>&) = delete;

    ~BufferPool<T>()
    {
        isActive_ = false;
        {
            OSAL::ScopedLock lock(mutex_);
            cv_.NotifyOne();
        }
        FinishAllocInProgress();
    }

    static std::shared_ptr<BufferPool<T>> Create(size_t poolSize)
    {
        return std::make_shared<BufferPool<T>>(poolSize);
    }

    void Init(size_t msgSize = DEFAULT_FRAME_SIZE, Plugin::BufferMetaType type = Plugin::BufferMetaType::AUDIO,
              size_t align = 0)
    {
        if (msgSize_ == msgSize && align == align_) {
            return;
        }
        metaType_ = type;
        msgSize_ = msgSize;
        align_ = align;
        freeBuffers_.clear();
        for (size_t i = 0; i < poolSize_; ++i) {
            auto buf = new Plugin::Buffer(type);
            buf->AllocMemory(nullptr, msgSize);
            freeBuffers_.emplace_back(std::unique_ptr<T>(buf));
        }
    }

    bool Append(std::unique_ptr<T> buffer)
    {
        OSAL::ScopedLock lock(mutex_);
        if (!isActive_ || freeBuffers_.size() >= poolSize_) {
            return false;
        }
        freeBuffers_.emplace_back(std::move(buffer));
        cv_.NotifyOne();
        return true;
    }

    void SetActive(bool active)
    {
        OSAL::ScopedLock lock(mutex_);
        if (!active) {
            cv_.NotifyOne();
        }
        isActive_ = active;
    }

    void RecycleBuffer(std::unique_ptr<T> buffer) const
    {
        OSAL::ScopedLock lock(mutex_);
        freeBuffers_.emplace_back(std::move(buffer));
        cv_.NotifyOne();
    }

    std::shared_ptr<T> AllocateBuffer()
    {
        OSAL::ScopedLock lock(mutex_);
        allocInProgress = true;
        if (freeBuffers_.empty() && isActive_) {
            cv_.Wait(lock, [this] { return !isActive_ || !freeBuffers_.empty(); });
        }
        std::shared_ptr<T> buffer;
        if (isActive_) {
            buffer = AllocateBufferUnprotected();
        }
        allocInProgress = false;
        cvFinishAlloc_.NotifyOne();
        return buffer;
    }

    std::shared_ptr<T> AllocateBufferNonBlocking()
    {
        OSAL::ScopedLock lock(mutex_);
        if (freeBuffers_.empty() || !isActive_) {
            return nullptr;
        }
        return AllocateBufferUnprotected();
    }

    std::shared_ptr<T> AllocateAppendBufferNonBlocking()
    {
        OSAL::ScopedLock lock(mutex_);
        if (!isActive_) {
            return nullptr;
        }
        if (freeBuffers_.empty()) {
            poolSize_++;
            auto buf = new Plugin::Buffer(metaType_);
            buf->AllocMemory(nullptr, msgSize_);
            freeBuffers_.emplace_back(std::unique_ptr<T>(buf));
        }
        return AllocateBufferUnprotected();
    }

    size_t Size() const
    {
        OSAL::ScopedLock lock(mutex_);
        return freeBuffers_.size();
    }

    size_t Capacity() const
    {
        return poolSize_;
    }

    bool Empty() const
    {
        OSAL::ScopedLock lock(mutex_);
        return freeBuffers_.empty();
    }

private:
    std::shared_ptr<T> AllocateBufferUnprotected()
    {
        std::weak_ptr<BufferPool<T>> weakRef(this->shared_from_this());
        std::shared_ptr<T> sptr(freeBuffers_.front().release(), [weakRef](T* ptr) {
            auto pool = weakRef.lock();
            if (pool) {
                pool->RecycleBuffer(std::unique_ptr<T>(ptr));
            } else {
                delete ptr;
            }
        });
        freeBuffers_.pop_front();
        return sptr;
    }

    void FinishAllocInProgress()
    {
        OSAL::ScopedLock lock(mutex_);
        if (allocInProgress.load()) {
            cvFinishAlloc_.Wait(lock, [this] { return !allocInProgress.load(); });
        }
    }

    mutable OSAL::Mutex mutex_;
    mutable OSAL::ConditionVariable cv_;
    mutable OSAL::ConditionVariable cvFinishAlloc_;
    mutable std::list<std::unique_ptr<T>> freeBuffers_;
    size_t poolSize_ {0};
    size_t msgSize_ {0};
    size_t align_ {0}; // 0: use default alignment.
    std::atomic<bool> isActive_;
    std::atomic<bool> allocInProgress;
    Plugin::BufferMetaType metaType_;
};
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_BUFFER_POOL_H
