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
#ifndef HDI_SHARED_MEM_QUEUE_META_H
#define HDI_SHARED_MEM_QUEUE_META_H

#include <atomic>
#include <cstddef>
#include <memory>
#include <message_parcel.h>
#include <unistd.h>

#ifndef HDF_LOG_TAG
#define HDF_LOG_TAG smq
#endif

namespace OHOS {
namespace HDI {
namespace Base {
enum SmqType : uint32_t {
    SYNCED_SMQ = 0x01,
    UNSYNC_SMQ = 0x02,
};

struct MemZone {
    uint32_t size;
    uint32_t offset;
};

template <typename T>
class SharedMemQueueMeta {
public:
    SharedMemQueueMeta() : SharedMemQueueMeta(-1, 0, 0) {};
    SharedMemQueueMeta(size_t elementCount, SmqType type) : SharedMemQueueMeta(-1, elementCount, type) {};
    SharedMemQueueMeta(int fd, size_t elementCount, SmqType type);
    SharedMemQueueMeta(const SharedMemQueueMeta<T> &other);
    ~SharedMemQueueMeta() = default;

    void SetFd(int fd);
    int GetFd();
    size_t GetSize();
    uint32_t GetType();
    size_t GetElementCount();
    size_t GetElemenetSize() const;
    enum MemZoneType : uint32_t {
        MEMZONE_RPTR = 0,
        MEMZONE_WPTR,
        MEMZONE_SYNCER,
        MEMZONE_DATA,
        MEMZONE_COUNT,
    };

    MemZone *GetMemZone(uint32_t type);
    bool Marshalling(MessageParcel &parcel);
    static std::shared_ptr<SharedMemQueueMeta<T>> UnMarshalling(MessageParcel &parcel);

    size_t AlignToWord(size_t num)
    {
        constexpr uint32_t alignByteSize = 8;
        return (num + alignByteSize - 1) & (~(alignByteSize - 1));
    }

private:
    int ashmemFd_;
    size_t size_;
    size_t elementCount_;
    size_t elementSize_;
    SmqType type_;
    MemZone memzone_[MEMZONE_COUNT];
};

template <typename T>
SharedMemQueueMeta<T>::SharedMemQueueMeta(int fd, size_t elementCount, SmqType type)
    : ashmemFd_(fd), size_(0), elementCount_(elementCount), elementSize_(AlignToWord(sizeof(T))), type_(type)
{
    // max size UIN32_MAX byte
    if (elementCount_ > UINT32_MAX / elementSize_) {
        return;
    }

    size_t dataSize = elementCount_ * elementSize_;
    size_t memZoneSize[] = {
        sizeof(uint64_t), // read ptr
        sizeof(uint64_t), // write ptr
        sizeof(uint32_t), // sync word
        dataSize,
    };

    size_t offset = 0;
    for (size_t i = 0; i < MEMZONE_COUNT; offset += memZoneSize[i++]) {
        memzone_[i].offset = AlignToWord(offset);
        memzone_[i].size = memZoneSize[i];
    }

    size_ = memzone_[MEMZONE_DATA].offset + memzone_[MEMZONE_DATA].size;
}

template <typename T>
SharedMemQueueMeta<T>::SharedMemQueueMeta(const SharedMemQueueMeta<T> &other)
{
    if (ashmemFd_ >= 0) {
        close(ashmemFd_);
    }

    ashmemFd_ = dup(other.ashmemFd_);
    if (ashmemFd_ < 0) {
        HDF_LOGW("failed to dup ashmem fd for smq");
    }
    elementCount_ = other.elementCount_;
    elementSize_ = other.elementSize_;
    size_ = other.size_;
    type_ = other.type_;
    memcpy(memzone_, other.memzone_, sizeof(memzone_));
}

template <typename T>
void SharedMemQueueMeta<T>::SetFd(int fd)
{
    if (ashmemFd_ >= 0) {
        close(ashmemFd_);
    }
    ashmemFd_ = fd;
}

template <typename T>
int SharedMemQueueMeta<T>::GetFd()
{
    return ashmemFd_;
}

template <typename T>
size_t SharedMemQueueMeta<T>::GetSize()
{
    return size_;
}

template <typename T>
uint32_t SharedMemQueueMeta<T>::GetType()
{
    return type_;
}

template <typename T>
size_t SharedMemQueueMeta<T>::GetElementCount()
{
    return elementCount_;
}

template <typename T>
size_t SharedMemQueueMeta<T>::GetElemenetSize() const
{
    return elementSize_;
}

template <typename T>
MemZone *SharedMemQueueMeta<T>::GetMemZone(uint32_t type)
{
    if (type >= MEMZONE_COUNT) {
        return nullptr;
    }

    return &memzone_[type];
}

template <typename T>
bool SharedMemQueueMeta<T>::Marshalling(MessageParcel &parcel)
{
    if (!parcel.WriteBuffer(this, sizeof(SharedMemQueueMeta<T>))) {
        return false;
    }

    return parcel.WriteFileDescriptor(ashmemFd_);
}

template <typename T>
std::shared_ptr<SharedMemQueueMeta<T>> SharedMemQueueMeta<T>::UnMarshalling(MessageParcel &parcel)
{
    auto readMeta = reinterpret_cast<const SharedMemQueueMeta<T> *>(parcel.ReadBuffer(sizeof(SharedMemQueueMeta<T>)));
    if (readMeta == nullptr) {
        HDF_LOGE("read invalid smq meta");
        return nullptr;
    }

    auto fd = parcel.ReadFileDescriptor();
    if (fd < 0) {
        HDF_LOGE("read invalid smq fd");
        return nullptr;
    }

    auto meta = std::make_shared<SharedMemQueueMeta<T>>(*readMeta);
    meta->SetFd(fd);

    return meta;
}
} // namespace Base
} // namespace HDI
} // namespace OHOS

#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif

#endif /* HDI_SHARED_MEM_QUEUE_META_H */
