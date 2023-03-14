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

#ifndef HISTREAMER_RING_BUFFER_H
#define HISTREAMER_RING_BUFFER_H

#include <atomic>
#include <memory>
#include "foundation/log.h"
#include "foundation/cpp_ext/memory_ext.h"
#include "foundation/osal/thread/condition_variable.h"
#include "foundation/osal/thread/mutex.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "securec.h"

namespace OHOS {
namespace Media {
class RingBuffer {
public:
    explicit RingBuffer(size_t bufferSize) : bufferSize_(bufferSize)
    {
    }

    ~RingBuffer() = default;

    bool Init()
    {
        buffer_ = CppExt::make_unique<uint8_t[]>(bufferSize_);
        return buffer_ != nullptr;
    }

    size_t ReadBuffer(void* ptr, size_t readSize, int waitTimes = 0)
    {
        OSAL::ScopedLock lck(writeMutex_);
        if (!isActive_) {
            return 0;
        }
        auto available = tail_ - head_;
        while (waitTimes > 0 && available == 0) {
            writeCondition_.Wait(lck);
            if (!isActive_) {
                return 0;
            }
            available = tail_ - head_;
            waitTimes--;
        }
        available = (available > readSize) ? readSize : available;
        size_t index = head_ % bufferSize_;
        if (index + available < bufferSize_) {
            (void)memcpy_s(ptr, available, buffer_.get() + index, available);
        } else {
            (void)memcpy_s(ptr, bufferSize_ - index, buffer_.get() + index, bufferSize_ - index);
            (void)memcpy_s(((uint8_t*)ptr) + (bufferSize_ - index), available - (bufferSize_ - index), buffer_.get(),
                           available - (bufferSize_ - index));
        }
        head_ += available;
        mediaOffset_ += available;
        writeCondition_.NotifyOne();
        return available;
    }

    void WriteBuffer(void* ptr, size_t writeSize, uint64_t mediaOffset = 0)
    {
        OSAL::ScopedLock lck(writeMutex_);
        if (!isActive_) {
            return;
        }
        while (writeSize + tail_ > head_ + bufferSize_) {
            writeCondition_.Wait(lck);
            if (!isActive_) {
                return;
            }
        }
        size_t index = tail_ % bufferSize_;
        if (index + writeSize < bufferSize_) {
            (void)memcpy_s(buffer_.get() + index, writeSize, ptr, writeSize);
        } else {
            (void)memcpy_s(buffer_.get() + index, bufferSize_ - index, ptr, bufferSize_ - index);
            (void)memcpy_s(buffer_.get(), writeSize - (bufferSize_ - index), ((uint8_t*)ptr) + bufferSize_ - index,
                           writeSize - (bufferSize_ - index));
        }
        tail_ += writeSize;
        if (head_ == 0) {
            mediaOffset_ = mediaOffset;
        }
        writeCondition_.NotifyOne();
    }

    void SetActive(bool active)
    {
        OSAL::ScopedLock lck(writeMutex_);
        isActive_ = active;
        if (!active) {
            head_ = 0;
            tail_ = 0;
            writeCondition_.NotifyOne();
        }
    }

    size_t GetSize()
    {
        return (tail_ - head_);
    }

    void Clear()
    {
        OSAL::ScopedLock lck(writeMutex_);
        head_ = 0;
        tail_ = 0;
        writeCondition_.NotifyOne();
    }

    bool Seek(uint64_t offset)
    {
        OSAL::ScopedLock lck(writeMutex_);
        MEDIA_LOG_I("Seek: buffer size " PUBLIC_LOG_ZU ", offset " PUBLIC_LOG_U64
                    ", mediaOffset_ " PUBLIC_LOG_U64, GetSize(), offset, mediaOffset_);
        bool result = false;
        if (offset >= mediaOffset_ && offset - mediaOffset_ < GetSize()) {
            head_ += offset - mediaOffset_;
            result = true;
        }
        writeCondition_.NotifyOne();
        return result;
    }
private:
    const size_t bufferSize_;
    std::unique_ptr<uint8_t[]> buffer_;
    size_t head_ {0}; // head
    size_t tail_ {0}; // tail
    OSAL::Mutex writeMutex_ {};
    OSAL::ConditionVariable writeCondition_ {};
    bool isActive_ {true};
    uint64_t mediaOffset_ {0};
};
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_RING_BUFFER_H
