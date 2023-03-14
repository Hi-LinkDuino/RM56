/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_sbuf.h"
#include "hdf_log.h"
#include "hdf_sbuf_impl.h"
#include "osal_mem.h"

#define HDF_SBUF_DEFAULT_SIZE 256
#define HDF_SBUF_IMPL_CHECK_RETURN(sbuf, api, retCode)               \
    do {                                                             \
        if ((sbuf) == NULL || (sbuf)->impl == NULL) {                    \
            HDF_LOGE("%s: invalid sbuf object", __func__);           \
            return retCode;                                          \
        }                                                            \
        if ((sbuf)->impl->api == NULL) {                               \
            HDF_LOGE(#api " is not supported on %u sbuf", (sbuf)->type); \
            return retCode;                                          \
        }                                                            \
    } while (0)

#define HDF_SBUF_IMPL_CHECK_RETURN_VOID(sbuf, api)                   \
    do {                                                             \
        if ((sbuf) == NULL || (sbuf)->impl == NULL) {                    \
            HDF_LOGE("%s: invalid sbuf object", __func__);           \
            return;                                                  \
        }                                                            \
        if ((sbuf)->impl->api == NULL) {                               \
            HDF_LOGE(#api " is not supported on %u sbuf", (sbuf)->type); \
            return;                                                  \
        }                                                            \
    } while (0)

struct HdfSBuf {
    struct HdfSBufImpl *impl;
    uint32_t type;
};

struct HdfSBufImpl *SbufObtainRaw(size_t capacity);
struct HdfSBufImpl *SbufBindRaw(uintptr_t base, size_t size);
struct HdfSBufImpl *SbufObtainIpc(size_t capacity) __attribute__((weak));
struct HdfSBufImpl *SbufBindIpc(uintptr_t base, size_t size) __attribute__((weak));
struct HdfSBufImpl *SbufObtainIpcHw(size_t capacity) __attribute__((weak));
struct HdfSBufImpl *SbufBindRawIpcHw(uintptr_t base, size_t size) __attribute__((weak));

static const struct HdfSbufConstructor g_sbufConstructorMap[SBUF_TYPE_MAX] = {
    [SBUF_RAW] = {
        .obtain = SbufObtainRaw,
        .bind = SbufBindRaw,
    },
    [SBUF_IPC] = {
        .obtain = SbufObtainIpc,
        .bind = SbufBindIpc,
    },
    [SBUF_IPC_HW] = {
        .obtain = SbufObtainIpcHw,
        .bind = SbufBindRawIpcHw,
    },
};

static const struct HdfSbufConstructor *HdfSbufConstructorGet(uint32_t type)
{
    if (type >= SBUF_TYPE_MAX) {
        return NULL;
    }

    return &g_sbufConstructorMap[type];
}

uint8_t *HdfSbufGetData(const struct HdfSBuf *sbuf)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, getData, NULL);
    return (uint8_t *)sbuf->impl->getData(sbuf->impl);
}

void HdfSbufFlush(struct HdfSBuf *sbuf)
{
    HDF_SBUF_IMPL_CHECK_RETURN_VOID(sbuf, getData);
    sbuf->impl->flush(sbuf->impl);
}

size_t HdfSbufGetCapacity(const struct HdfSBuf *sbuf)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, getCapacity, HDF_FAILURE);
    return (sbuf != NULL && sbuf->impl != NULL) ? sbuf->impl->getCapacity(sbuf->impl) : 0;
}

size_t HdfSbufGetDataSize(const struct HdfSBuf *sbuf)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, getDataSize, HDF_FAILURE);
    return sbuf->impl->getDataSize(sbuf->impl);
}

void HdfSbufSetDataSize(struct HdfSBuf *sbuf, size_t size)
{
    HDF_SBUF_IMPL_CHECK_RETURN_VOID(sbuf, setDataSize);
    sbuf->impl->setDataSize(sbuf->impl, size);
}

bool HdfSbufWriteBuffer(struct HdfSBuf *sbuf, const void *data, uint32_t writeSize)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, getCapacity, false);
    return sbuf->impl->writeBuffer(sbuf->impl, (const uint8_t *)data, writeSize);
}

bool HdfSbufWriteUnpadBuffer(struct HdfSBuf *sbuf, const uint8_t *data, uint32_t writeSize)
{
    if (data == NULL) {
        return false;
    }

    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeUnpadBuffer, false);
    return sbuf->impl->writeUnpadBuffer(sbuf->impl, data, writeSize);
}

const uint8_t *HdfSbufReadUnpadBuffer(struct HdfSBuf *sbuf, size_t length)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readUnpadBuffer, NULL);
    return sbuf->impl->readUnpadBuffer(sbuf->impl, length);
}

bool HdfSbufReadBuffer(struct HdfSBuf *sbuf, const void **data, uint32_t *readSize)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readBuffer, false);
    return sbuf->impl->readBuffer(sbuf->impl, (const uint8_t **)data, readSize);
}

bool HdfSbufWriteUint64(struct HdfSBuf *sbuf, uint64_t value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeUint64, false);
    return sbuf->impl->writeUint64(sbuf->impl, value);
}

bool HdfSbufWriteUint32(struct HdfSBuf *sbuf, uint32_t value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeUint32, false);
    return sbuf->impl->writeUint32(sbuf->impl, value);
}

bool HdfSbufWriteUint16(struct HdfSBuf *sbuf, uint16_t value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeUint16, false);
    return sbuf->impl->writeUint16(sbuf->impl, value);
}

bool HdfSbufWriteUint8(struct HdfSBuf *sbuf, uint8_t value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeUint8, false);
    return sbuf->impl->writeUint8(sbuf->impl, value);
}

bool HdfSbufWriteInt64(struct HdfSBuf *sbuf, int64_t value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeInt64, false);
    return sbuf->impl->writeInt64(sbuf->impl, value);
}

bool HdfSbufWriteInt32(struct HdfSBuf *sbuf, int32_t value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeInt32, false);
    return sbuf->impl->writeInt32(sbuf->impl, value);
}

bool HdfSbufWriteInt16(struct HdfSBuf *sbuf, int16_t value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeInt16, false);
    return sbuf->impl->writeInt16(sbuf->impl, value);
}

bool HdfSbufWriteInt8(struct HdfSBuf *sbuf, int8_t value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeInt8, false);
    return sbuf->impl->writeInt8(sbuf->impl, value);
}

bool HdfSbufWriteString(struct HdfSBuf *sbuf, const char *value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeString, false);
    return sbuf->impl->writeString(sbuf->impl, value);
}

bool HdfSbufWriteString16(struct HdfSBuf *sbuf, const char16_t *value, uint32_t size)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeString16, false);
    return sbuf->impl->writeString16(sbuf->impl, value, size);
}

bool HdfSbufReadUint64(struct HdfSBuf *sbuf, uint64_t *value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readUint64, false);
    return sbuf->impl->readUint64(sbuf->impl, value);
}

bool HdfSbufReadUint32(struct HdfSBuf *sbuf, uint32_t *value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readUint32, false);
    return sbuf->impl->readUint32(sbuf->impl, value);
}

bool HdfSbufReadUint16(struct HdfSBuf *sbuf, uint16_t *value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readUint16, false);
    return sbuf->impl->readUint16(sbuf->impl, value);
}

bool HdfSbufReadUint8(struct HdfSBuf *sbuf, uint8_t *value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readUint8, false);
    return sbuf->impl->readUint8(sbuf->impl, value);
}

bool HdfSbufReadInt64(struct HdfSBuf *sbuf, int64_t *value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readInt64, false);
    return sbuf->impl->readInt64(sbuf->impl, value);
}

bool HdfSbufReadInt32(struct HdfSBuf *sbuf, int32_t *value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readInt32, false);
    return sbuf->impl->readInt32(sbuf->impl, value);
}

bool HdfSbufReadInt16(struct HdfSBuf *sbuf, int16_t *value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readInt16, false);
    return sbuf->impl->readInt16(sbuf->impl, value);
}

bool HdfSbufReadInt8(struct HdfSBuf *sbuf, int8_t *value)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readInt8, false);
    return sbuf->impl->readInt8(sbuf->impl, value);
}

const char *HdfSbufReadString(struct HdfSBuf *sbuf)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readString, NULL);
    return sbuf->impl->readString(sbuf->impl);
}

bool HdfSBufWriteString16(struct HdfSBuf *sbuf, const char16_t *value, uint32_t size)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeString16, false);
    return sbuf->impl->writeString16(sbuf->impl, value, size);
}

const char16_t *HdfSbufReadString16(struct HdfSBuf *sbuf)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readString16, NULL);
    return sbuf->impl->readString16(sbuf->impl);
}

int32_t HdfSbufWriteRemoteService(struct HdfSBuf *sbuf, const struct HdfRemoteService *service)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeRemoteService, false);
    return sbuf->impl->writeRemoteService(sbuf->impl, service);
}

struct HdfRemoteService *HdfSbufReadRemoteService(struct HdfSBuf *sbuf)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readRemoteService, NULL);
    return sbuf->impl->readRemoteService(sbuf->impl);
}

bool HdfSbufWriteFloat(struct HdfSBuf *sbuf, float data)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeFloat, false);
    return sbuf->impl->writeFloat(sbuf->impl, data);
}

bool HdfSbufWriteDouble(struct HdfSBuf *sbuf, double data)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeDouble, false);
    return sbuf->impl->writeDouble(sbuf->impl, data);
}

bool HdfSbufWriteFileDescriptor(struct HdfSBuf *sbuf, int fd)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, writeFileDescriptor, false);
    return sbuf->impl->writeFileDescriptor(sbuf->impl, fd);
}

int HdfSbufReadFileDescriptor(struct HdfSBuf *sbuf)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readFileDescriptor, false);
    return sbuf->impl->readFileDescriptor(sbuf->impl);
}

bool HdfSbufReadDouble(struct HdfSBuf *sbuf, double *data)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readDouble, false);
    return sbuf->impl->readDouble(sbuf->impl, data);
}

bool HdfSbufReadFloat(struct HdfSBuf *sbuf, float *data)
{
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, readFloat, false);
    return sbuf->impl->readFloat(sbuf->impl, data);
}

struct HdfSBuf *HdfSbufTypedObtainCapacity(uint32_t type, size_t capacity)
{
    struct HdfSBuf *sbuf = NULL;
    const struct HdfSbufConstructor *constructor = HdfSbufConstructorGet(type);
    if (constructor == NULL) {
        HDF_LOGE("sbuf constructor %u not implement", type);
        return NULL;
    }
    if (constructor->obtain == NULL) {
        HDF_LOGE("sbuf constructor %u obtain method not implement", type);
        return NULL;
    }

    sbuf = (struct HdfSBuf *)OsalMemAlloc(sizeof(struct HdfSBuf));
    if (sbuf == NULL) {
        HDF_LOGE("instance sbuf failure");
        return NULL;
    }

    sbuf->impl = constructor->obtain(capacity);
    if (sbuf->impl == NULL) {
        OsalMemFree(sbuf);
        HDF_LOGE("sbuf obtain fail, size=%u", (uint32_t)capacity);
        return NULL;
    }
    sbuf->type = type;
    return sbuf;
}

struct HdfSBuf *HdfSbufTypedObtainInplace(uint32_t type, struct HdfSBufImpl *impl)
{
    struct HdfSBuf *sbuf = NULL;
    if (type >= SBUF_TYPE_MAX || impl == NULL) {
        return NULL;
    }

    sbuf = (struct HdfSBuf *)OsalMemAlloc(sizeof(struct HdfSBuf));
    if (sbuf == NULL) {
        HDF_LOGE("obtain in-place sbuf failure");
        return NULL;
    }

    sbuf->impl = impl;
    sbuf->type = type;
    return sbuf;
}

struct HdfSBuf *HdfSbufTypedObtain(uint32_t type)
{
    return HdfSbufTypedObtainCapacity(type, HDF_SBUF_DEFAULT_SIZE);
}

struct HdfSBuf *HdfSbufTypedBind(uint32_t type, uintptr_t base, size_t size)
{
    struct HdfSBuf *sbuf = NULL;
    const struct HdfSbufConstructor *constructor = HdfSbufConstructorGet(type);
    if (constructor == NULL) {
        HDF_LOGE("sbuf constructor %u not implement", type);
        return NULL;
    }

    if (constructor->bind == NULL) {
        HDF_LOGE("sbuf constructor %u bind method not implement", type);
        return NULL;
    }

    sbuf = (struct HdfSBuf *)OsalMemAlloc(sizeof(struct HdfSBuf));
    if (sbuf == NULL) {
        HDF_LOGE("instance sbuf failure");
        return NULL;
    }

    sbuf->impl = constructor->bind(base, size);
    if (sbuf->impl == NULL) {
        OsalMemFree(sbuf);
        HDF_LOGE("sbuf bind fail");
        return NULL;
    }
    sbuf->type = type;
    return sbuf;
}

struct HdfSBuf *HdfSbufObtain(size_t capacity)
{
    return HdfSbufTypedObtainCapacity(SBUF_RAW, capacity);
}

struct HdfSBuf *HdfSbufObtainDefaultSize(void)
{
    return HdfSbufObtain(HDF_SBUF_DEFAULT_SIZE);
}

struct HdfSBuf *HdfSBufObtainDefaultSize(void)
{
    return HdfSbufObtain(HDF_SBUF_DEFAULT_SIZE);
}

struct HdfSBuf *HdfSbufBind(uintptr_t base, size_t size)
{
    return HdfSbufTypedBind(SBUF_RAW, base, size);
}

struct HdfSBuf *HdfSbufCopy(const struct HdfSBuf *sbuf)
{
    struct HdfSBuf *newBuf = NULL;
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, copy, NULL);
    newBuf = (struct HdfSBuf *)OsalMemAlloc(sizeof(struct HdfSBuf));
    if (newBuf == NULL) {
        return NULL;
    }
    newBuf->impl = sbuf->impl->copy(sbuf->impl);
    if (newBuf->impl == NULL) {
        OsalMemFree(newBuf);
        return NULL;
    }
    newBuf->type = sbuf->type;
    return newBuf;
}

struct HdfSBuf *HdfSbufMove(struct HdfSBuf *sbuf)
{
    struct HdfSBuf *newBuf = NULL;
    HDF_SBUF_IMPL_CHECK_RETURN(sbuf, move, NULL);
    newBuf = (struct HdfSBuf *)OsalMemAlloc(sizeof(struct HdfSBuf));
    if (newBuf == NULL) {
        return NULL;
    }
    newBuf->impl = sbuf->impl->move(sbuf->impl);
    if (newBuf->impl == NULL) {
        OsalMemFree(newBuf);
        return NULL;
    }
    return newBuf;
}

void HdfSbufTransDataOwnership(struct HdfSBuf *sbuf)
{
    HDF_SBUF_IMPL_CHECK_RETURN_VOID(sbuf, transDataOwnership);
    sbuf->impl->transDataOwnership(sbuf->impl);
}

void HdfSbufRecycle(struct HdfSBuf *sbuf)
{
    if (sbuf != NULL) {
        if (sbuf->impl != NULL && sbuf->impl->recycle != NULL) {
            sbuf->impl->recycle(sbuf->impl);
            sbuf->impl = NULL;
        }
        OsalMemFree(sbuf);
    }
}

void HdfSBufRecycle(struct HdfSBuf *sbuf)
{
    if (sbuf != NULL) {
        if (sbuf->impl != NULL && sbuf->impl->recycle != NULL) {
            sbuf->impl->recycle(sbuf->impl);
            sbuf->impl = NULL;
        }
        OsalMemFree(sbuf);
    }
}

struct HdfSBufImpl *HdfSbufGetImpl(struct HdfSBuf *sbuf)
{
    if (sbuf != NULL) {
        return sbuf->impl;
    }

    return NULL;
}