/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HDI_SHARED_MEM_QUEUE_INF_H
#define HDI_SHARED_MEM_QUEUE_INF_H

#include <ashmem.h>
#include <atomic>
#include <cerrno>
#include <datetime_ex.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdi_smq_meta.h>
#include <hdi_smq_syncer.h>
#include <memory>
#include <securec.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#ifndef HDF_LOG_TAG
#define HDF_LOG_TAG smq
#endif

namespace OHOS {
namespace HDI {
namespace Base {
template <typename T>
class SharedMemQueue {
public:
    SharedMemQueue(uint32_t elementCount, SmqType type);
    explicit SharedMemQueue(const SharedMemQueueMeta<T> &meta);
    ~SharedMemQueue();

    int Write(const T *data, size_t count);
    int Read(T *data, size_t count);

    int Write(const T *data);
    int Read(T *data);

    int Write(const T *data, size_t count, int64_t waitTimeNanoSec);
    int Read(T *data, size_t count, int64_t waitTimeNanoSec);

    int WriteNonBlocking(const T *data);
    int ReadNonBlocking(T *data);

    int WriteNonBlocking(const T *data, size_t count);
    int ReadNonBlocking(T *data, size_t count);

    size_t GetAvalidWriteSize();
    size_t GetAvalidReadSize();
    size_t GetSize();
    std::shared_ptr<SharedMemQueueMeta<T>> GetMeta();
    bool IsGood();
    static inline int64_t GetNanoTime()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * SEC_TO_NANOSEC + ts.tv_nsec);
    }

private:
    void Init(bool resetWriteOffset);
    uintptr_t MapMemZone(uint32_t zoneType);
    void UnMapMemZone(void *addr, uint32_t zoneType);
    size_t Align(size_t num, size_t alignSize);

    int32_t status = HDF_FAILURE;
    size_t alignedElmtSize_;
    uint8_t *queueBuffer_ = nullptr;
    std::atomic<uint64_t> *readOffset_ = nullptr;
    std::atomic<uint64_t> *writeOffset_ = nullptr;
    std::atomic<uint32_t> *syncerPtr_ = nullptr;
    std::unique_ptr<SharedMemQueueSyncer> syncer_ = nullptr;
    std::shared_ptr<SharedMemQueueMeta<T>> meta_ = nullptr;
};

template <typename T>
SharedMemQueue<T>::SharedMemQueue(uint32_t elementCount, SmqType type) : alignedElmtSize_(0)
{
    if (elementCount > UINT16_MAX) {
        return;
    }

    meta_ = std::make_shared<SharedMemQueueMeta<T>>(elementCount, type);
    HDF_LOGI("create SharedMemQueue, count=%{public}u, size=%{public}u", elementCount, meta_->GetSize());
    int ashmemFd = AshmemCreate("hdi_smq", Align(meta_->GetSize(), PAGE_SIZE));
    if (ashmemFd < 0) {
        HDF_LOGE("failed to create ashmem");
        return;
    }
    meta_->SetFd(ashmemFd);
    alignedElmtSize_ = meta_->GetElemenetSize();
    Init(true);
}

template <typename T>
SharedMemQueue<T>::SharedMemQueue(const SharedMemQueueMeta<T> &meta) : alignedElmtSize_(meta.GetElemenetSize())
{
    meta_ = std::make_shared<SharedMemQueueMeta<T>>(meta);
    Init(false);
}

template <typename T>
SharedMemQueue<T>::~SharedMemQueue()
{
    if (meta_ != nullptr && meta_->GetType() == SYNCED_SMQ && readOffset_ != nullptr) {
        UnMapMemZone(readOffset_, SharedMemQueueMeta<T>::MemZoneType::MEMZONE_RPTR);
    } else {
        delete readOffset_;
        readOffset_ = nullptr;
    }

    if (writeOffset_ != nullptr) {
        UnMapMemZone(writeOffset_, SharedMemQueueMeta<T>::MEMZONE_WPTR);
    }

    if (syncerPtr_ != nullptr) {
        UnMapMemZone(syncerPtr_, SharedMemQueueMeta<T>::MEMZONE_SYNCER);
    }

    if (queueBuffer_ != nullptr) {
        UnMapMemZone(queueBuffer_, SharedMemQueueMeta<T>::MEMZONE_DATA);
    }
}

template <typename T>
void SharedMemQueue<T>::Init(bool resetWriteOffset)
{
    if (meta_ == nullptr || meta_->GetElemenetSize() != sizeof(T)) {
        HDF_LOGE("invalid smq meta for init");
        return;
    }

    if (meta_->GetType() == SYNCED_SMQ) {
        readOffset_ = reinterpret_cast<std::atomic<uint64_t> *>(MapMemZone(SharedMemQueueMeta<T>::MEMZONE_RPTR));
    } else {
        readOffset_ = new std::atomic<uint64_t>;
    }

    if (readOffset_ == nullptr) {
        HDF_LOGE("failed to map read offset");
        return;
    }

    writeOffset_ = reinterpret_cast<std::atomic<uint64_t> *>(MapMemZone(SharedMemQueueMeta<T>::MEMZONE_WPTR));
    if (writeOffset_ == nullptr) {
        HDF_LOGE("failed to map write offset");
        return;
    }

    syncerPtr_ = reinterpret_cast<std::atomic<uint32_t> *>(MapMemZone(SharedMemQueueMeta<T>::MEMZONE_SYNCER));
    if (syncerPtr_ == nullptr) {
        HDF_LOGE("failed to map sync ptr");
        return;
    }

    queueBuffer_ = reinterpret_cast<uint8_t *>(MapMemZone(SharedMemQueueMeta<T>::MEMZONE_DATA));
    if (queueBuffer_ == nullptr) {
        HDF_LOGE("failed to map queue buffer");
        return;
    }

    syncer_ = std::make_unique<SharedMemQueueSyncer>(syncerPtr_);

    if (resetWriteOffset) {
        writeOffset_->store(0, std::memory_order_release);
    }
    readOffset_->store(0, std::memory_order_release);
    HDF_LOGI("smq init succ");
    status = HDF_SUCCESS;
}

template <typename T>
uintptr_t SharedMemQueue<T>::MapMemZone(uint32_t zoneType)
{
    auto memzone = meta_->GetMemZone(zoneType);
    if (memzone == nullptr) {
        HDF_LOGE("invalid smq mem zone type %{public}u", zoneType);
        return reinterpret_cast<uintptr_t>(nullptr);
    }

    int offset = (memzone->offset / PAGE_SIZE) * PAGE_SIZE;
    int length = memzone->offset - offset + memzone->size;

    void *ptr = mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED, meta_->GetFd(), offset);
    if (ptr == MAP_FAILED) {
        HDF_LOGE(
            "failed to map memzone %{public}u, size %{public}u, offset %{public}u , fd %{public}d, errnor=%{public}d",
            zoneType, length, offset, meta_->GetFd(), errno);
        return reinterpret_cast<uintptr_t>(nullptr);
    }
    return (reinterpret_cast<uintptr_t>(ptr) + (memzone->offset - offset));
}

template <typename T>
void SharedMemQueue<T>::UnMapMemZone(void *addr, uint32_t zoneType)
{
    auto memzone = meta_->GetMemZone(zoneType);
    if (memzone == nullptr) {
        return;
    }
    int offset = (memzone->offset / PAGE_SIZE) * PAGE_SIZE;
    int length = memzone->offset - offset + memzone->size;
    uint8_t *ptr = reinterpret_cast<uint8_t *>(addr) - (memzone->offset - offset);
    if (ptr == nullptr) {
        return;
    }
    munmap(ptr, length);
}

template <typename T>
bool SharedMemQueue<T>::IsGood()
{
    return status == HDF_SUCCESS;
}

template <typename T>
size_t SharedMemQueue<T>::Align(size_t num, size_t alignSize)
{
    return (num + alignSize - 1) & ~(alignSize - 1);
}

template <typename T>
int SharedMemQueue<T>::Write(const T *data, size_t count)
{
    return Write(data, count, 0);
}

template <typename T>
int SharedMemQueue<T>::Read(T *data, size_t count)
{
    return Read(data, count, 0);
}

template <typename T>
int SharedMemQueue<T>::Write(const T *data)
{
    return Write(data, 1, 0);
}

template <typename T>
int SharedMemQueue<T>::Read(T *data)
{
    return Read(data, 1, 0);
}

template <typename T>
int SharedMemQueue<T>::Write(const T *data, size_t count, int64_t waitTimeNanoSec)
{
    if (meta_->GetType() != SmqType::SYNCED_SMQ) {
        HDF_LOGE("unsynecd smq not support blocking write");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (WriteNonBlocking(data, count) == 0) {
        return syncer_->Wake(SharedMemQueueSyncer::SYNC_WORD_READ);
    }

    int ret = 0;
    auto startTime = GetNanoTime();
    uint64_t currentTime = startTime;
    while (true) {
        if (waitTimeNanoSec != 0) {
            currentTime = GetNanoTime();
            if (GetNanoTime() - startTime >= waitTimeNanoSec) {
                ret = WriteNonBlocking(data, count);
                break;
            }
            waitTimeNanoSec -= currentTime - startTime;
        }
        ret = syncer_->Wait(SharedMemQueueSyncer::SYNC_WORD_WRITE, waitTimeNanoSec);
        if (ret != 0 && ret != -ETIMEDOUT) {
            break;
        }

        ret = WriteNonBlocking(data, count);
        if (ret == 0) {
            break;
        }
        HDF_LOGE("failed to write %{public}zu, retry", count);
    }

    if (ret == 0) {
        ret = syncer_->Wake(SharedMemQueueSyncer::SYNC_WORD_READ);
    } else {
        HDF_LOGE("failed to write %{public}zu, ret=%{public}d", count, ret);
    }

    return ret;
}

template <typename T>
int SharedMemQueue<T>::Read(T *data, size_t count, int64_t waitTimeNanoSec)
{
    if (meta_->GetType() != SmqType::SYNCED_SMQ) {
        HDF_LOGE("unsynecd smq not support blocking read");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (ReadNonBlocking(data, count) == 0) {
        return syncer_->Wake(SharedMemQueueSyncer::SYNC_WORD_WRITE);
    }

    int ret = -ENODATA;
    auto startTime = GetNanoTime();
    int64_t currentTime;
    while (true) {
        if (waitTimeNanoSec != 0) {
            currentTime = GetNanoTime();
            if (currentTime - startTime >= waitTimeNanoSec) {
                ret = ReadNonBlocking(data, count);
                break;
            }
            waitTimeNanoSec -= currentTime - startTime;
        }
        ret = syncer_->Wait(SharedMemQueueSyncer::SYNC_WORD_READ, waitTimeNanoSec);
        if (ret != 0 && ret != -ETIMEDOUT) {
            break;
        }

        ret = ReadNonBlocking(data, count);
        if (ret == 0) {
            break;
        }
    }
    if (ret == 0) {
        ret = syncer_->Wake(SharedMemQueueSyncer::SYNC_WORD_WRITE);
    } else {
        HDF_LOGE("failed to read %{public}zu, ret=%{public}d", count, ret);
    }

    return ret;
}

template <typename T>
int SharedMemQueue<T>::WriteNonBlocking(const T *data)
{
    return WriteNonBlocking(data, 1);
}

template <typename T>
int SharedMemQueue<T>::ReadNonBlocking(T *data)
{
    return ReadNonBlocking(data, 1);
}

template <typename T>
int SharedMemQueue<T>::WriteNonBlocking(const T *data, size_t count)
{
    auto avalidWrite = GetAvalidWriteSize();
    if (count >= avalidWrite && meta_->GetType() == SmqType::SYNCED_SMQ) {
        // synced smq can not overflow write
        return -E2BIG;
    }

    auto wOffset = writeOffset_->load(std::memory_order_acquire);
    auto rOffset = readOffset_->load(std::memory_order_acquire);
    uint64_t newWriteOffset;
    auto qCount = meta_->GetElementCount();
    if (wOffset + count <= qCount) {
        if (memcpy_s(queueBuffer_ + (wOffset * sizeof(T)), (qCount - wOffset) * sizeof(T),
            data, count * sizeof(T)) != EOK) {
            return HDF_FAILURE;
        };
        newWriteOffset = (wOffset + count) % qCount;
    } else {
        size_t firstPartSize = qCount - wOffset;
        size_t secParcSize = count - firstPartSize;
        if (memcpy_s(queueBuffer_ + (wOffset * sizeof(T)), (qCount - wOffset) * sizeof(T),
            data, firstPartSize * sizeof(T)) != EOK) {
            return HDF_FAILURE;
        }
        if (memcpy_s(queueBuffer_, qCount * sizeof(T), data + firstPartSize, secParcSize * sizeof(T)) != EOK) {
            return HDF_FAILURE;
        }
        newWriteOffset = secParcSize;
    }

    writeOffset_->store(newWriteOffset, std::memory_order_release);
    if (wOffset < rOffset && newWriteOffset >= rOffset) {
        HDF_LOGW("warning:smp ring buffer overflow");
    }
    return 0;
}

template <typename T>
int SharedMemQueue<T>::ReadNonBlocking(T *data, size_t count)
{
    if (count == 0) {
        return -EINVAL;
    }

    if (count > GetAvalidReadSize()) {
        return -ENODATA;
    }

    auto qCount = meta_->GetElementCount();
    auto rOffset = readOffset_->load(std::memory_order_acquire);
    if (rOffset + count <= qCount) {
        if (memcpy_s(data, count * sizeof(T), queueBuffer_ + (rOffset * sizeof(T)), count * sizeof(T)) != EOK) {
            return HDF_FAILURE;
        }
        readOffset_->store((rOffset + count) % qCount, std::memory_order_release);
        return 0;
    }

    size_t firstPartSize = qCount - rOffset;
    size_t secPartSize = count - firstPartSize;

    if (memcpy_s(data, count * sizeof(T), queueBuffer_ + (rOffset * sizeof(T)), firstPartSize * sizeof(T)) != EOK) {
        return HDF_FAILURE;
    }
    if (memcpy_s(data + firstPartSize, (count - firstPartSize) * sizeof(T),
        queueBuffer_, secPartSize * sizeof(T)) != EOK) {
        return HDF_FAILURE;
    };
    readOffset_->store(secPartSize, std::memory_order_release);

    return 0;
}

template <typename T>
size_t SharedMemQueue<T>::GetAvalidWriteSize()
{
    return meta_->GetElementCount() - GetAvalidReadSize();
}

template <typename T>
size_t SharedMemQueue<T>::GetAvalidReadSize()
{
    auto wOffset = writeOffset_->load(std::memory_order_acquire);
    auto rOffset = readOffset_->load(std::memory_order_acquire);
    auto size = wOffset >= rOffset ? (wOffset - rOffset) : (wOffset + meta_->GetElementCount() - rOffset);
    return size;
}

template <typename T>
size_t SharedMemQueue<T>::GetSize()
{
    return meta_->GetSize();
}

template <typename T>
std::shared_ptr<SharedMemQueueMeta<T>> SharedMemQueue<T>::GetMeta()
{
    return meta_;
}
} // namespace Base
} // namespace HDI
} // namespace OHOS

#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif

#endif /* HDI_SHARED_MEM_QUEUEHDI_INF_H */
