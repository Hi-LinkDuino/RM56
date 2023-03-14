/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "message_parcel.h"

#include <sys/mman.h>
#include <unistd.h>

#include "ashmem.h"
#ifndef CONFIG_IPC_SINGLE
#include "dbinder_callback_stub.h"
#include "dbinder_session_object.h"
#endif
#include "ipc_debug.h"
#include "ipc_file_descriptor.h"
#include "ipc_process_skeleton.h"
#include "iremote_object.h"
#include "log_tags.h"
#include "securec.h"
#include "sys_binder.h"

namespace OHOS {
#ifndef TITLE
#define TITLE __PRETTY_FUNCTION__
#endif

#ifndef CONFIG_IPC_SINGLE
static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, LOG_ID_RPC, "MessageParcel" };
#define DBINDER_LOGE(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "%{public}s %{public}d: " fmt, TITLE, __LINE__, ##args)
#define DBINDER_LOGI(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "%{public}s %{public}d: " fmt, TITLE, __LINE__, ##args)
#endif


MessageParcel::MessageParcel()
    : Parcel(),
      writeRawDataFd_(-1),
      readRawDataFd_(-1),
      kernelMappedWrite_(nullptr),
      kernelMappedRead_(nullptr),
      rawData_(nullptr),
      rawDataSize_(0)
{}

MessageParcel::MessageParcel(Allocator *allocator)
    : Parcel(allocator),
      writeRawDataFd_(-1),
      readRawDataFd_(-1),
      kernelMappedWrite_(nullptr),
      kernelMappedRead_(nullptr),
      rawData_(nullptr),
      rawDataSize_(0)
{}

MessageParcel::~MessageParcel()
{
    if (kernelMappedWrite_ != nullptr) {
        ::munmap(kernelMappedWrite_, rawDataSize_);
        kernelMappedWrite_ = nullptr;
    }
    if (kernelMappedRead_ != nullptr) {
        ::munmap(kernelMappedRead_, rawDataSize_);
        kernelMappedRead_ = nullptr;
    }

    if (readRawDataFd_ > 0) {
        ::close(readRawDataFd_);
        readRawDataFd_ = -1;
    }
    if (writeRawDataFd_ > 0) {
        ::close(writeRawDataFd_);
        writeRawDataFd_ = -1;
    }

    ClearFileDescriptor();

    rawData_ = nullptr;
    rawDataSize_ = 0;
}


#ifndef CONFIG_IPC_SINGLE
bool MessageParcel::WriteDBinderProxy(const sptr<IRemoteObject> &object, uint32_t handle, uint64_t stubIndex)
{
    IPCProcessSkeleton *current = IPCProcessSkeleton::GetCurrent();
    if (current == nullptr) {
        DBINDER_LOGE("current is nullptr");
        return false;
    }
    std::shared_ptr<DBinderSessionObject> sessionOfPeer = current->ProxyQueryDBinderSession(handle);
    if (sessionOfPeer == nullptr) {
        DBINDER_LOGE("sessionOfPeer is nullptr");
        return false;
    }
    std::string peerName = sessionOfPeer->GetServiceName();
    std::string peerId = sessionOfPeer->GetDeviceId();
    std::string localId = current->GetLocalDeviceID();
    sptr<DBinderCallbackStub> fakeStub = current->QueryDBinderCallbackStub(object);
    if (fakeStub == nullptr) {
        // note that cannot use this proxy's descriptor
        fakeStub = new DBinderCallbackStub(peerName, peerId, localId, stubIndex, handle);
        if (!current->AttachDBinderCallbackStub(object, fakeStub)) {
            DBINDER_LOGE("save callback of fake stub failed");
            return false;
        }
    }
    return WriteRemoteObject(fakeStub);
}
#endif

bool MessageParcel::WriteRemoteObject(const sptr<IRemoteObject> &object)
{
    if (object == nullptr) {
        return false;
    }
    holders_.push_back(object);
#ifndef CONFIG_IPC_SINGLE
    if (object->IsProxyObject()) {
        const IPCObjectProxy *proxy = reinterpret_cast<const IPCObjectProxy *>(object.GetRefPtr());
        const uint32_t handle = proxy ? proxy->GetHandle() : 0;
        IPCProcessSkeleton *current = IPCProcessSkeleton::GetCurrent();
        if (IPCProcessSkeleton::IsHandleMadeByUser(handle) && current != nullptr) {
            DBINDER_LOGI("send dbinder object to local devices");
            /* this is a fake proxy which handle get by MakeRemoteHandle(), Not binder driver of kernel */
            uint64_t stubIndex = current->QueryHandleToIndex(handle);
            if (stubIndex > 0) {
                DBINDER_LOGI("this is dbinder proxy want to send anthor process in this device");
                return WriteDBinderProxy(object, handle, stubIndex);
            }
        }
    }
#endif
    auto result = WriteObject<IRemoteObject>(object);
    if (result == false) {
        return result;
    }
    return result;
}

sptr<IRemoteObject> MessageParcel::ReadRemoteObject()
{
    sptr<IRemoteObject> temp = ReadObject<IRemoteObject>();
#ifndef CONFIG_IPC_SINGLE
    if (temp != nullptr && !temp->IsProxyObject()) {
        // if this stub is a DBinderCallbackStub, return corresponding proxy
        IPCProcessSkeleton *current = IPCProcessSkeleton::GetCurrent();
        if (current != nullptr) {
            sptr<IRemoteObject> proxy = current->QueryDBinderCallbackProxy(temp);
            if (proxy != nullptr) {
                temp = proxy;
            }
        }
    }
#endif
    return temp;
}

bool MessageParcel::WriteFileDescriptor(int fd)
{
    if (fd < 0) {
        return false;
    }
    int dupFd = dup(fd);
    if (dupFd < 0) {
        return false;
    }
    sptr<IPCFileDescriptor> descriptor = new IPCFileDescriptor(dupFd);
    return WriteObject<IPCFileDescriptor>(descriptor);
}

int MessageParcel::ReadFileDescriptor()
{
    sptr<IPCFileDescriptor> descriptor = ReadObject<IPCFileDescriptor>();
    if (descriptor == nullptr) {
        return -1;
    }
    int fd = descriptor->GetFd();
    if (fd < 0) {
        return -1;
    }
    holders_.push_back(descriptor);
    return dup(fd);
}

void MessageParcel::ClearFileDescriptor()
{
    binder_size_t *object = reinterpret_cast<binder_size_t *>(GetObjectOffsets());
    size_t objectNum = GetOffsetsSize();
    uintptr_t data = GetData();
    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object *flat = reinterpret_cast<flat_binder_object *>(data + object[i]);
        if (flat->hdr.type == BINDER_TYPE_FD && flat->handle > 0) {
            ::close(flat->handle);
        }
    }
}

bool MessageParcel::ContainFileDescriptors() const
{
    binder_size_t *object = reinterpret_cast<binder_size_t *>(GetObjectOffsets());
    size_t objectNum = GetOffsetsSize();
    uintptr_t data = GetData();
    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object *flat = reinterpret_cast<flat_binder_object *>(data + object[i]);
        if (flat->hdr.type == BINDER_TYPE_FD) {
            return true;
        }
    }

    return false;
}

bool MessageParcel::WriteInterfaceToken(std::u16string name)
{
    constexpr int strictModePolicy = 0x100;
    constexpr int workSource = 0;
    size_t rewindPos = GetWritePosition();
    if (!WriteInt32(strictModePolicy)) {
        return false;
    }

    if (!WriteInt32(workSource)) {
        if (!RewindWrite(rewindPos)) {
            FlushBuffer();
        }
        return false;
    }

    return WriteString16(name);
}

std::u16string MessageParcel::ReadInterfaceToken()
{
    [[maybe_unused]] int strictModePolicy = ReadInt32();
    [[maybe_unused]] int workSource = ReadInt32();
    return ReadString16();
}

bool MessageParcel::WriteRawData(const void *data, size_t size)
{
    if (data == nullptr || size > MAX_RAWDATA_SIZE) {
        return false;
    }
    if (kernelMappedWrite_ != nullptr) {
        return false;
    }
    if (!WriteInt32(size)) {
        return false;
    }
    if (size <= MIN_RAWDATA_SIZE) {
        return WriteUnpadBuffer(data, size);
    }
    int fd = AshmemCreate("Parcel RawData", size);
    if (fd < 0) {
        return false;
    }
    writeRawDataFd_ = fd;

    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        return false;
    }
    void *ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        return false;
    }
    if (!WriteFileDescriptor(fd)) {
        ::munmap(ptr, size);
        return false;
    }
    if (memcpy_s(ptr, size, data, size) != EOK) {
        ::munmap(ptr, size);
        return false;
    }
    kernelMappedWrite_ = ptr;
    rawDataSize_ = size;
    return true;
}

bool MessageParcel::RestoreRawData(std::shared_ptr<char> rawData, size_t size)
{
    if (rawData_ != nullptr || rawData == nullptr) {
        return false;
    }
    rawData_ = rawData;
    rawDataSize_ = size;
    writeRawDataFd_ = 0;
    return true;
}

const void *MessageParcel::ReadRawData(size_t size)
{
    int32_t bufferSize = ReadInt32();
    if (static_cast<unsigned int>(bufferSize) != size) {
        return nullptr;
    }

    if (static_cast<unsigned int>(bufferSize) <= MIN_RAWDATA_SIZE) {
        return ReadUnpadBuffer(size);
    }

    /* if rawDataFd_ == 0 means rawData is received from remote
     */
    if (rawData_ != nullptr && writeRawDataFd_ == 0) {
        /* should read fd for move readCursor of parcel */
        if (ReadFileDescriptor()) {
            // do nothing
        }
        if (rawDataSize_ != size) {
            return nullptr;
        }
        return rawData_.get();
    }
    int fd = ReadFileDescriptor();
    if (fd < 0) {
        return nullptr;
    }
    readRawDataFd_ = fd;

    int ashmemSize = AshmemGetSize(fd);
    if (ashmemSize < 0 || size_t(ashmemSize) < size) {
        // do not close fd here. fd will be closed in FileDescriptor, ::close(fd)
        return nullptr;
    }
    void *ptr = ::mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        // do not close fd here. fd will be closed in FileDescriptor, ::close(fd)
        return nullptr;
    }
    kernelMappedRead_ = ptr;
    rawDataSize_ = size;
    return ptr;
}

const void *MessageParcel::GetRawData() const
{
    if (rawData_ != nullptr) {
        return rawData_.get();
    }
    if (kernelMappedWrite_ != nullptr) {
        return kernelMappedWrite_;
    }
    if (kernelMappedRead_ != nullptr) {
        return kernelMappedRead_;
    }
    return nullptr;
}

size_t MessageParcel::GetRawDataSize() const
{
    return rawDataSize_;
}

size_t MessageParcel::GetRawDataCapacity() const
{
    return MAX_RAWDATA_SIZE;
}

void MessageParcel::WriteNoException()
{
    WriteInt32(0);
}

int32_t MessageParcel::ReadException()
{
    int32_t errorCode = ReadInt32();
    if (errorCode != 0) {
        ReadString16();
    }
    return errorCode;
}

bool MessageParcel::WriteAshmem(sptr<Ashmem> ashmem)
{
    int fd = ashmem->GetAshmemFd();
    int32_t size = ashmem->GetAshmemSize();
    if (fd < 0 || size <= 0) {
        return false;
    }
    if (!WriteFileDescriptor(fd) || !WriteInt32(size)) {
        return false;
    }
    return true;
}

sptr<Ashmem> MessageParcel::ReadAshmem()
{
    int fd = ReadFileDescriptor();
    if (fd < 0) {
        return nullptr;
    }

    int32_t size = ReadInt32();
    if (size <= 0) {
        ::close(fd);
        return nullptr;
    }
    return new Ashmem(fd, size);
}
} // namespace OHOS
