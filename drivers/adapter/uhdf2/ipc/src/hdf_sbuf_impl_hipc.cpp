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

#include <vector>
#include <string>
#include "hdf_sbuf_ipc.h"
#include "hdf_sbuf_impl.h"
#include "hdf_log.h"
#include "hdf_remote_adapter.h"

using namespace OHOS;

static void MParcelImplInterfaceAssign(struct HdfSBufImpl *inf);

struct SBufMParcelImpl {
    SBufMParcelImpl(OHOS::MessageParcel *parcel, bool owned = true): realParcel_(parcel), owned_(owned)
    {
        MParcelImplInterfaceAssign(&infImpl);
    }
    ~SBufMParcelImpl()
    {
        if (owned_ && realParcel_ != nullptr) {
            delete realParcel_;
            realParcel_ = nullptr;
        }
    }
    struct HdfSBufImpl infImpl;
    OHOS::MessageParcel *realParcel_;
    std::vector<std::u16string> str16Pool_;
    bool owned_;
};

static MessageParcel *MParcelCast(struct HdfSBufImpl *impl)
{
    SBufMParcelImpl *sbufImpl = reinterpret_cast<SBufMParcelImpl *>(impl);
    return sbufImpl->realParcel_;
}

static bool SbufMParcelImplWriteBuffer(struct HdfSBufImpl *sbuf, const uint8_t *data, uint32_t writeSize)
{
    if (sbuf == nullptr) {
        return false;
    }
    auto parcel = MParcelCast(sbuf);
    if (!parcel->WriteUint32(writeSize)) {
        return false;
    }
    return parcel->WriteUnpadBuffer((const void *)data, writeSize);
}

static bool SbufMParcelImplWriteUnpadBuffer(struct HdfSBufImpl *sbuf, const uint8_t *data, uint32_t writeSize)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteUnpadBuffer(static_cast<const void *>(data), writeSize);
}

static bool SbufMParcelImplWriteUint64(struct HdfSBufImpl *sbuf, uint64_t value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteUint64(value);
}

static bool SbufMParcelImplWriteUint32(struct HdfSBufImpl *sbuf, uint32_t value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteUint32(value);
}

static bool SbufMParcelImplWriteUint16(struct HdfSBufImpl *sbuf, uint16_t value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteUint16(value);
}

static bool SbufMParcelImplWriteUint8(struct HdfSBufImpl *sbuf, uint8_t value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteUint8(value);
}

static bool SbufMParcelImplWriteInt64(struct HdfSBufImpl *sbuf, int64_t value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteInt64(value);
}

static bool SbufMParcelImplWriteInt32(struct HdfSBufImpl *sbuf, int32_t value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteInt32(value);
}

static bool SbufMParcelImplWriteInt16(struct HdfSBufImpl *sbuf, int16_t value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteInt16(value);
}

static bool SbufMParcelImplWriteInt8(struct HdfSBufImpl *sbuf, int8_t value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteInt8(value);
}

static bool SbufMParcelImplWriteString(struct HdfSBufImpl *sbuf, const char *value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteCString(value);
}

static bool SbufMParcelImplWriteString16(struct HdfSBufImpl *sbuf, const char16_t *value, uint32_t size)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteString16WithLength(value, size);
}

static bool SbufMParcelImplWriteFileDescriptor(struct HdfSBufImpl *sbuf, int fd)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteFileDescriptor(fd);
}

static bool SbufMParcelImplWriteDouble(struct HdfSBufImpl *sbuf, double value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteDouble(value);
}

static bool SbufMParcelImplWriteFloat(struct HdfSBufImpl *sbuf, float value)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->WriteFloat(value);
}

static bool SbufMParcelImplReadFloat(struct HdfSBufImpl *sbuf, float *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    float v = 0;
    bool ret = MParcelCast(sbuf)->ReadFloat(v);
    *value = v;
    return ret;
}

static bool SbufMParcelImplReadDouble(struct HdfSBufImpl *sbuf, double *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    double v = 0;
    bool ret = MParcelCast(sbuf)->ReadDouble(v);
    *value = v;
    return ret;
}

static int SbufMParcelImplReadFileDescriptor(struct HdfSBufImpl *sbuf)
{
    if (sbuf == nullptr) {
        return false;
    }
    return MParcelCast(sbuf)->ReadFileDescriptor();
}

static bool SbufMParcelImplReadBuffer(struct HdfSBufImpl *sbuf, const uint8_t **data, uint32_t *readSize)
{
    if (sbuf == nullptr || data == nullptr || readSize == nullptr) {
        return false;
    }
    MessageParcel *parcel = MParcelCast(sbuf);
    *readSize = parcel->ReadUint32();
    *data = parcel->ReadUnpadBuffer(*readSize);
    return *data != nullptr;
}

static const uint8_t *SbufMParcelImplReadUnpadBuffer(struct HdfSBufImpl *sbuf, size_t length)
{
    if (sbuf == nullptr) {
        return nullptr;
    }
    return MParcelCast(sbuf)->ReadUnpadBuffer(length);
}

static bool SbufMParcelImplReadUint64(struct HdfSBufImpl *sbuf, uint64_t *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    uint64_t v = 0;
    bool ret = MParcelCast(sbuf)->ReadUint64(v);
    *value = v;
    return ret;
}

static bool SbufMParcelImplReadUint32(struct HdfSBufImpl *sbuf, uint32_t *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    uint32_t v = 0;
    bool ret = MParcelCast(sbuf)->ReadUint32(v);
    *value = v;
    return ret;
}

static bool SbufMParcelImplReadUint16(struct HdfSBufImpl *sbuf, uint16_t *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    uint16_t v = 0;
    bool ret = MParcelCast(sbuf)->ReadUint16(v);
    *value = v;
    return ret;
}

static bool SbufMParcelImplReadUint8(struct HdfSBufImpl *sbuf, uint8_t *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    uint8_t v = 0;
    bool ret = MParcelCast(sbuf)->ReadUint8(v);
    *value = v;
    return ret;
}

static bool SbufMParcelImplReadInt64(struct HdfSBufImpl *sbuf, int64_t *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    int64_t v = 0;
    bool ret = MParcelCast(sbuf)->ReadInt64(v);
    *value = v;
    return ret;
}

static bool SbufMParcelImplReadInt32(struct HdfSBufImpl *sbuf, int32_t *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    int32_t v = 0;
    bool ret = MParcelCast(sbuf)->ReadInt32(v);
    *value = v;
    return ret;
}

static bool SbufMParcelImplReadInt16(struct HdfSBufImpl *sbuf, int16_t *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    int16_t v = 0;
    bool ret = MParcelCast(sbuf)->ReadInt16(v);
    *value = v;
    return ret;
}

static bool SbufMParcelImplReadInt8(struct HdfSBufImpl *sbuf, int8_t *value)
{
    if (sbuf == nullptr || value == nullptr) {
        return false;
    }
    int8_t v = 0;
    bool ret = MParcelCast(sbuf)->ReadInt8(v);
    *value = v;
    return ret;
}

static const char *SbufMParcelImplReadString(struct HdfSBufImpl *sbuf)
{
    if (sbuf == nullptr) {
        return nullptr;
    }
    return MParcelCast(sbuf)->ReadCString();
}

static const char16_t *SbufMParcelImplReadString16(struct HdfSBufImpl *sbuf)
{
    if (sbuf == nullptr) {
        return nullptr;
    }
    SBufMParcelImpl *sbufImpl = reinterpret_cast<SBufMParcelImpl *>(sbuf);
    sbufImpl->str16Pool_.emplace_back(sbufImpl->realParcel_->ReadString16());
    return sbufImpl->str16Pool_.rbegin()->c_str();
}

static int32_t SbufMParcelImplWriteRemoteService(struct HdfSBufImpl *sbuf, const struct HdfRemoteService *service)
{
    if (sbuf == nullptr || service == nullptr) {
        return false;
    }
    MessageParcel *parcel = MParcelCast(sbuf);
    const struct HdfRemoteServiceHolder *holder = reinterpret_cast<const struct HdfRemoteServiceHolder *>(service);
    if (holder != nullptr) {
        return parcel->WriteRemoteObject(holder->remote_) ? HDF_SUCCESS : HDF_FAILURE;
    }
    return HDF_FAILURE;
}

static struct HdfRemoteService *SbufMParcelImplReadRemoteService(struct HdfSBufImpl *sbuf)
{
    if (sbuf == nullptr) {
        return nullptr;
    }
    auto remote = MParcelCast(sbuf)->ReadRemoteObject();
    if (remote == nullptr) {
        HDF_LOGE("%{public}s: read remote object fail", __func__);
        return nullptr;
    }
    return HdfRemoteAdapterBind(remote);
}

static const uint8_t *SbufMParcelImplGetData(const struct HdfSBufImpl *sbuf)
{
    if (sbuf == nullptr) {
        return nullptr;
    }
    return reinterpret_cast<const uint8_t *>(MParcelCast(const_cast<struct HdfSBufImpl *>(sbuf))->GetData());
}

static void SbufMParcelImplFlush(struct HdfSBufImpl *sbuf)
{
    if (sbuf == nullptr) {
        return;
    }
    return MParcelCast(sbuf)->FlushBuffer();
}

static size_t SbufMParcelImplGetCapacity(const struct HdfSBufImpl *sbuf)
{
    if (sbuf == nullptr) {
        return HDF_FAILURE;
    }

    return MParcelCast(const_cast<struct HdfSBufImpl *>(sbuf))->GetDataCapacity();
}

static size_t SbufMParcelImplGetDataSize(const struct HdfSBufImpl *sbuf)
{
    if (sbuf == nullptr) {
        return HDF_FAILURE;
    }
    return MParcelCast(const_cast<struct HdfSBufImpl *>(sbuf))->GetDataSize();
}

static void SbufMParcelImplSetDataSize(struct HdfSBufImpl *sbuf, size_t size)
{
    if (sbuf == nullptr) {
        return;
    }
    MParcelCast(sbuf)->SetDataSize(size);
}

static void SbufMParcelImplRecycle(struct HdfSBufImpl *sbuf)
{
    if (sbuf == nullptr) {
        return;
    }
    SBufMParcelImpl *sbufImpl = reinterpret_cast<SBufMParcelImpl *>(sbuf);
    delete sbufImpl;
}

static void MParcelImplInterfaceAssign(struct HdfSBufImpl *inf)
{
    inf->writeBuffer = SbufMParcelImplWriteBuffer;
    inf->writeUnpadBuffer = SbufMParcelImplWriteUnpadBuffer;
    inf->writeUint64 = SbufMParcelImplWriteUint64;
    inf->writeUint32 = SbufMParcelImplWriteUint32;
    inf->writeUint16 = SbufMParcelImplWriteUint16;
    inf->writeUint8 = SbufMParcelImplWriteUint8;
    inf->writeInt64 = SbufMParcelImplWriteInt64;
    inf->writeInt32 = SbufMParcelImplWriteInt32;
    inf->writeInt16 = SbufMParcelImplWriteInt16;
    inf->writeInt8 = SbufMParcelImplWriteInt8;
    inf->writeString = SbufMParcelImplWriteString;
    inf->writeString16 = SbufMParcelImplWriteString16;
    inf->writeFileDescriptor = SbufMParcelImplWriteFileDescriptor;
    inf->writeFloat = SbufMParcelImplWriteFloat;
    inf->writeDouble = SbufMParcelImplWriteDouble;
    inf->readDouble = SbufMParcelImplReadDouble;
    inf->readFloat = SbufMParcelImplReadFloat;
    inf->readFileDescriptor = SbufMParcelImplReadFileDescriptor;
    inf->readBuffer = SbufMParcelImplReadBuffer;
    inf->readUnpadBuffer = SbufMParcelImplReadUnpadBuffer;
    inf->readUint64 = SbufMParcelImplReadUint64;
    inf->readUint32 = SbufMParcelImplReadUint32;
    inf->readUint16 = SbufMParcelImplReadUint16;
    inf->readUint8 = SbufMParcelImplReadUint8;
    inf->readInt64 = SbufMParcelImplReadInt64;
    inf->readInt32 = SbufMParcelImplReadInt32;
    inf->readInt16 = SbufMParcelImplReadInt16;
    inf->readInt8 = SbufMParcelImplReadInt8;
    inf->readString = SbufMParcelImplReadString;
    inf->readString16 = SbufMParcelImplReadString16;
    inf->writeRemoteService = SbufMParcelImplWriteRemoteService;
    inf->readRemoteService = SbufMParcelImplReadRemoteService;
    inf->getData = SbufMParcelImplGetData;
    inf->flush = SbufMParcelImplFlush;
    inf->getCapacity = SbufMParcelImplGetCapacity;
    inf->getDataSize = SbufMParcelImplGetDataSize;
    inf->setDataSize = SbufMParcelImplSetDataSize;
    inf->recycle = SbufMParcelImplRecycle;
    inf->transDataOwnership = nullptr;
    inf->copy = nullptr;
    inf->move = nullptr;
}

extern "C" struct HdfSBufImpl *SbufObtainIpc(size_t capacity)
{
    (void)capacity;
    struct SBufMParcelImpl *sbuf = new SBufMParcelImpl(new MessageParcel());
    return &sbuf->infImpl;
}

class SbufAllocator : public Allocator {
public:
    void *Realloc(void *data, size_t newSize) override
    {
        (void)data;
        (void)newSize;
        return nullptr;
    }

    void *Alloc(size_t size) override
    {
        (void)size;
        return nullptr;
    }

    void Dealloc(void *data) override
    {
        (void)data;
    }
};

extern "C" struct HdfSBufImpl *SbufBindIpc(uintptr_t base, size_t size)
{
    struct SBufMParcelImpl *sbuf = new SBufMParcelImpl(new MessageParcel(new SbufAllocator()));
    sbuf->realParcel_->ParseFrom(base, size);
    return &sbuf->infImpl;
}

struct HdfSBuf *ParcelToSbuf(OHOS::MessageParcel *parcel)
{
    struct SBufMParcelImpl *sbuf = new SBufMParcelImpl(parcel, false);
    return HdfSbufTypedObtainInplace(SBUF_IPC, &sbuf->infImpl);
}

int32_t SbufToParcel(struct HdfSBuf *sbuf, OHOS::MessageParcel **parcel)
{
    if (sbuf == nullptr || parcel == nullptr) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct SBufMParcelImpl *impl = reinterpret_cast<SBufMParcelImpl *>(HdfSbufGetImpl(sbuf));
    if (impl == nullptr) {
        return HDF_ERR_INVALID_OBJECT;
    }

    *parcel = impl->realParcel_;
    return HDF_SUCCESS;
}