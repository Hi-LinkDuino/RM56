/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "hdf_sbuf_impl.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_SBUF_GROW_SIZE_DEFAULT 256
#define HDF_SBUF_MAX_SIZE (512 * 1024) // 512KB
#define HDF_SBUF_ALIGN 4

#ifndef INT16_MAX
#ifdef S16_MAX
#define INT16_MAX S16_MAX
#else
#define INT16_MAX 32767
#endif // !S16_MAX
#endif // INT16_MAX

struct HdfSBufRaw {
    struct HdfSBufImpl infImpl;
    size_t writePos; /**< Current write position */
    size_t readPos;  /**< Current read position */
    size_t capacity; /**< Storage capacity, 512 KB at most. */
    uint8_t *data;   /**< Pointer to data storage */
    bool isBind;     /**< Whether to bind the externally transferred pointer to data storage */
};

#define SBUF_RAW_CAST(impl) (struct HdfSBufRaw *)(impl)

static struct HdfSBufRaw *SbufRawImplNewInstance(size_t capacity);
static void SbufInterfaceAssign(struct HdfSBufImpl *inf);

static size_t SbufRawImplGetAlignSize(size_t size)
{
    return (size + HDF_SBUF_ALIGN - 1) & (~(HDF_SBUF_ALIGN - 1));
}

static void SbufRawImplRecycle(struct HdfSBufImpl *impl)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    if (sbuf != NULL) {
        if (sbuf->data != NULL && !sbuf->isBind) {
            OsalMemFree(sbuf->data);
        }
        OsalMemFree(sbuf);
    }
}

static size_t SbufRawImplGetLeftWriteSize(struct HdfSBufRaw *sbuf)
{
    return (sbuf->capacity < sbuf->writePos) ? 0 : (sbuf->capacity - sbuf->writePos);
}

static size_t SbufRawImplGetLeftReadSize(struct HdfSBufRaw *sbuf)
{
    return (sbuf->writePos < sbuf->readPos) ? 0 : (sbuf->writePos - sbuf->readPos);
}

static bool SbufRawImplWriteRollback(struct HdfSBufImpl *impl, uint32_t size)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    size_t alignSize;
    if (sbuf == NULL) {
        return false;
    }

    alignSize = SbufRawImplGetAlignSize(size);
    if (sbuf->writePos < alignSize) {
        return false;
    }

    sbuf->writePos -= alignSize;
    return true;
}

static bool SbufRawImplReadRollback(struct HdfSBufImpl *impl, uint32_t size)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    size_t alignSize;
    if (sbuf == NULL) {
        return false;
    }

    alignSize = SbufRawImplGetAlignSize(size);
    if (sbuf->readPos < alignSize) {
        return false;
    }

    sbuf->readPos -= alignSize;
    return true;
}

static const uint8_t *SbufRawImplGetData(const struct HdfSBufImpl *impl)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    if (sbuf == NULL) {
        HDF_LOGE("The obtained data is null, and the input Sbuf is null.");
        return NULL;
    }
    return (uint8_t *)sbuf->data;
}

static void SbufRawImplSetDataSize(struct HdfSBufImpl *impl, size_t size)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    if (sbuf == NULL) {
        return;
    }
    if (size <= sbuf->capacity) {
        sbuf->readPos = 0;
        sbuf->writePos = size;
    }
}

static void SbufRawImplFlush(struct HdfSBufImpl *impl)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    if (sbuf != NULL) {
        sbuf->readPos = 0;
        sbuf->writePos = 0;
    }
}

static size_t SbufRawImplGetCapacity(const struct HdfSBufImpl *impl)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    return (sbuf != NULL) ? sbuf->capacity : 0;
}

static size_t SbufRawImplGetDataSize(const struct HdfSBufImpl *impl)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    return (sbuf != NULL) ? sbuf->writePos : 0;
}

static bool SbufRawImplGrow(struct HdfSBufRaw *sbuf, uint32_t growSize)
{
    uint32_t newSize;
    uint8_t *newData = NULL;
    if (sbuf->isBind) {
        HDF_LOGE("%s: binded sbuf oom", __func__);
        return false;
    }

    newSize = SbufRawImplGetAlignSize(sbuf->capacity + growSize);
    if (newSize < sbuf->capacity) {
        HDF_LOGE("%s: grow size overflow", __func__);
        return false;
    }
    if (newSize > HDF_SBUF_MAX_SIZE) {
        HDF_LOGE("%s: buf size over limit", __func__);
        return false;
    }

    newData = OsalMemCalloc(newSize);
    if (newData == NULL) {
        HDF_LOGE("%s: oom", __func__);
        return false;
    }

    if (sbuf->data != NULL) {
        if (memcpy_s(newData, newSize, sbuf->data, sbuf->writePos) != EOK) {
            OsalMemFree(newData);
            return false;
        }
        OsalMemFree(sbuf->data);
    }

    sbuf->data = newData;
    sbuf->capacity = newSize;

    return true;
}

static bool SbufRawImplWrite(struct HdfSBufImpl *impl, const uint8_t *data, uint32_t size)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    size_t alignSize;
    size_t writeableSize;
    uint8_t *dest = NULL;

    if (sbuf == NULL || sbuf->data == NULL || data == NULL) {
        return false;
    }

    if (size == 0) {
        return true;
    }

    alignSize = SbufRawImplGetAlignSize(size);
    // in case of desireCapacity overflow
    if (alignSize < size) {
        HDF_LOGE("desireCapacity overflow");
        return false;
    }
    writeableSize = SbufRawImplGetLeftWriteSize(sbuf);
    if (alignSize > writeableSize) {
        size_t growSize = (alignSize > HDF_SBUF_GROW_SIZE_DEFAULT) ? (alignSize + HDF_SBUF_GROW_SIZE_DEFAULT)
                                                                   : HDF_SBUF_GROW_SIZE_DEFAULT;
        if (!SbufRawImplGrow(sbuf, growSize)) {
            return false;
        }
        writeableSize = SbufRawImplGetLeftWriteSize(sbuf);
    }

    dest = sbuf->data + sbuf->writePos;
    if (memcpy_s(dest, writeableSize, data, size) != EOK) {
        return false; /* never hits */
    }

    sbuf->writePos += alignSize;
    return true;
}

static bool SbufRawImplRead(struct HdfSBufImpl *impl, uint8_t *data, uint32_t readSize)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    size_t alignSize;
    if (sbuf == NULL || sbuf->data == NULL || data == NULL) {
        return false;
    }

    if (readSize == 0) {
        return true;
    }

    alignSize = SbufRawImplGetAlignSize(readSize);
    if (alignSize > SbufRawImplGetLeftReadSize(sbuf)) {
        HDF_LOGE("Read out of buffer range");
        return false;
    }

    if (memcpy_s(data, readSize, sbuf->data + sbuf->readPos, readSize) != EOK) {
        return false; // never hit
    }
    sbuf->readPos += alignSize;
    return true;
}

static bool SbufRawImplWriteUint64(struct HdfSBufImpl *impl, uint64_t value)
{
    return SbufRawImplWrite(impl, (uint8_t *)(&value), sizeof(value));
}

static bool SbufRawImplWriteUint32(struct HdfSBufImpl *impl, uint32_t value)
{
    return SbufRawImplWrite(impl, (uint8_t *)(&value), sizeof(value));
}

static bool SbufRawImplWriteUint16(struct HdfSBufImpl *impl, uint16_t value)
{
    return SbufRawImplWrite(impl, (uint8_t *)(&value), sizeof(value));
}

static bool SbufRawImplWriteUint8(struct HdfSBufImpl *impl, uint8_t value)
{
    return SbufRawImplWrite(impl, (uint8_t *)(&value), sizeof(value));
}

static bool SbufRawImplWriteInt64(struct HdfSBufImpl *impl, int64_t value)
{
    return SbufRawImplWrite(impl, (uint8_t *)(&value), sizeof(value));
}

static bool SbufRawImplWriteInt32(struct HdfSBufImpl *impl, int32_t value)
{
    return SbufRawImplWrite(impl, (uint8_t *)(&value), sizeof(value));
}

static bool SbufRawImplWriteInt16(struct HdfSBufImpl *impl, int16_t value)
{
    return SbufRawImplWrite(impl, (uint8_t *)(&value), sizeof(value));
}

static bool SbufRawImplWriteInt8(struct HdfSBufImpl *impl, int8_t value)
{
    return SbufRawImplWrite(impl, (uint8_t *)(&value), sizeof(value));
}

static bool SbufRawImplWriteBuffer(struct HdfSBufImpl *impl, const uint8_t *data, uint32_t writeSize)
{
    if (impl == NULL) {
        HDF_LOGE("Failed to write the Sbuf, invalid input params");
        return false;
    }
    if (data == NULL) {
        return SbufRawImplWriteInt32(impl, 0);
    }

    if (!SbufRawImplWriteInt32(impl, writeSize)) {
        return false;
    }
    if (!SbufRawImplWrite(impl, data, writeSize)) {
        (void)SbufRawImplWriteRollback(impl, sizeof(int32_t));
        return false;
    }

    return true;
}

static bool SbufRawImplWriteString(struct HdfSBufImpl *impl, const char *value)
{
    if (impl == NULL) {
        HDF_LOGE("%s: input null", __func__);
        return false;
    }

    return SbufRawImplWriteBuffer(impl, (const uint8_t *)value, value ? (strlen(value) + 1) : 0);
}

static bool SbufRawImplReadUint64(struct HdfSBufImpl *impl, uint64_t *value)
{
    return SbufRawImplRead(impl, (uint8_t *)(value), sizeof(*value));
}

static bool SbufRawImplReadUint32(struct HdfSBufImpl *impl, uint32_t *value)
{
    return SbufRawImplRead(impl, (uint8_t *)(value), sizeof(*value));
}

static bool SbufRawImplReadUint16(struct HdfSBufImpl *impl, uint16_t *value)
{
    return SbufRawImplRead(impl, (uint8_t *)(value), sizeof(*value));
}

static bool SbufRawImplReadUint8(struct HdfSBufImpl *impl, uint8_t *value)
{
    return SbufRawImplRead(impl, (uint8_t *)(value), sizeof(*value));
}

static bool SbufRawImplReadInt64(struct HdfSBufImpl *impl, int64_t *value)
{
    return SbufRawImplRead(impl, (uint8_t *)(value), sizeof(*value));
}

static bool SbufRawImplReadInt32(struct HdfSBufImpl *impl, int32_t *value)
{
    return SbufRawImplRead(impl, (uint8_t *)(value), sizeof(*value));
}

static bool SbufRawImplReadInt16(struct HdfSBufImpl *impl, int16_t *value)
{
    return SbufRawImplRead(impl, (uint8_t *)(value), sizeof(*value));
}

static bool SbufRawImplReadInt8(struct HdfSBufImpl *impl, int8_t *value)
{
    return SbufRawImplRead(impl, (uint8_t *)(value), sizeof(*value));
}

static bool SbufRawImplReadBuffer(struct HdfSBufImpl *impl, const uint8_t **data, uint32_t *readSize)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    int buffSize = 0;
    size_t alignSize;
    if (sbuf == NULL || sbuf->data == NULL || data == NULL || readSize == NULL) {
        HDF_LOGE("%s: input invalid", __func__);
        return false;
    }

    if (!SbufRawImplReadInt32(impl, &buffSize)) {
        return false;
    }

    if (buffSize == 0) {
        *data = NULL;
        *readSize = 0;
        return true;
    }
    alignSize = SbufRawImplGetAlignSize(buffSize);
    if (alignSize > SbufRawImplGetLeftReadSize(sbuf)) {
        HDF_LOGE("%s:readBuff out of range", __func__);
        (void)SbufRawImplReadRollback(impl, sizeof(int32_t));
        return false;
    }

    *data = sbuf->data + sbuf->readPos;
    *readSize = buffSize;
    sbuf->readPos += alignSize;
    return true;
}

static const char *SbufRawImplReadString(struct HdfSBufImpl *impl)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    int32_t strLen = 0;
    size_t alignSize;
    char *str = NULL;
    if (sbuf == NULL || sbuf->data == NULL) {
        HDF_LOGE("%s: input null", __func__);
        return NULL;
    }
    /* This length contains the '\0' at the end of the string. */
    if (!SbufRawImplReadInt32(impl, &strLen) || strLen <= 0) {
        return NULL;
    }
    alignSize = SbufRawImplGetAlignSize(strLen);
    if (strLen > INT16_MAX || alignSize > SbufRawImplGetLeftReadSize(sbuf)) {
        (void)SbufRawImplReadRollback(impl, sizeof(int32_t));
        return NULL;
    }

    str = (char *)(sbuf->data + sbuf->readPos);
    sbuf->readPos += alignSize;
    /* Set '\0' at end of the string forcibly. */
    str[strLen - 1] = '\0';
    return str;
}

static struct HdfSBufImpl *SbufRawImplCopy(const struct HdfSBufImpl *impl)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    struct HdfSBufRaw *new = NULL;
    if (sbuf == NULL || sbuf->data == NULL) {
        return NULL;
    }

    new = SbufRawImplNewInstance(sbuf->capacity);
    if (new == NULL) {
        return NULL;
    }
    new->capacity = sbuf->capacity;
    new->readPos = 0;
    new->writePos = sbuf->writePos;
    if (memcpy_s(new->data, new->capacity, sbuf->data, sbuf->capacity) != EOK) {
        SbufRawImplRecycle(&new->infImpl);
        return NULL;
    }

    return &new->infImpl;
}

static struct HdfSBufImpl *SbufRawImplMove(struct HdfSBufImpl *impl)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    struct HdfSBufRaw *new = NULL;
    if (sbuf == NULL || sbuf->isBind) {
        return NULL;
    }

    new = OsalMemCalloc(sizeof(struct HdfSBufRaw));
    if (new == NULL) {
        return NULL;
    }
    new->capacity = sbuf->capacity;
    new->readPos = 0;
    new->writePos = sbuf->writePos;
    new->data = sbuf->data;

    sbuf->data = NULL;
    sbuf->capacity = 0;
    SbufRawImplFlush(&sbuf->infImpl);
    SbufInterfaceAssign(&new->infImpl);

    return &new->infImpl;
}

static void SbufRawImplTransDataOwnership(struct HdfSBufImpl *impl)
{
    struct HdfSBufRaw *sbuf = SBUF_RAW_CAST(impl);
    if (sbuf == NULL) {
        return;
    }

    sbuf->isBind = false;
}

static void SbufInterfaceAssign(struct HdfSBufImpl *inf)
{
    inf->writeBuffer = SbufRawImplWriteBuffer;
    inf->writeUint64 = SbufRawImplWriteUint64;
    inf->writeUint32 = SbufRawImplWriteUint32;
    inf->writeUint16 = SbufRawImplWriteUint16;
    inf->writeUint8 = SbufRawImplWriteUint8;
    inf->writeInt64 = SbufRawImplWriteInt64;
    inf->writeInt32 = SbufRawImplWriteInt32;
    inf->writeInt16 = SbufRawImplWriteInt16;
    inf->writeInt8 = SbufRawImplWriteInt8;
    inf->writeString = SbufRawImplWriteString;
    inf->readBuffer = SbufRawImplReadBuffer;
    inf->readUint64 = SbufRawImplReadUint64;
    inf->readUint32 = SbufRawImplReadUint32;
    inf->readUint16 = SbufRawImplReadUint16;
    inf->readUint8 = SbufRawImplReadUint8;
    inf->readInt64 = SbufRawImplReadInt64;
    inf->readInt32 = SbufRawImplReadInt32;
    inf->readInt16 = SbufRawImplReadInt16;
    inf->readInt8 = SbufRawImplReadInt8;
    inf->readString = SbufRawImplReadString;
    inf->getData = SbufRawImplGetData;
    inf->flush = SbufRawImplFlush;
    inf->getCapacity = SbufRawImplGetCapacity;
    inf->getDataSize = SbufRawImplGetDataSize;
    inf->setDataSize = SbufRawImplSetDataSize;
    inf->recycle = SbufRawImplRecycle;
    inf->move = SbufRawImplMove;
    inf->copy = SbufRawImplCopy;
    inf->transDataOwnership = SbufRawImplTransDataOwnership;
}

static struct HdfSBufRaw *SbufRawImplNewInstance(size_t capacity)
{
    struct HdfSBufRaw *sbuf = NULL;
    if (capacity > HDF_SBUF_MAX_SIZE) {
        HDF_LOGE("%s: Sbuf size exceeding max limit", __func__);
        return NULL;
    }
    sbuf = (struct HdfSBufRaw *)OsalMemCalloc(sizeof(struct HdfSBufRaw));
    if (sbuf == NULL) {
        HDF_LOGE("Sbuf instance failure");
        return NULL;
    }

    sbuf->data = (uint8_t *)OsalMemCalloc(capacity);
    if (sbuf->data == NULL) {
        OsalMemFree(sbuf);
        HDF_LOGE("sbuf obtain memory oom, size=%u", (uint32_t)capacity);
        return NULL;
    }
    sbuf->capacity = capacity;
    sbuf->writePos = 0;
    sbuf->readPos = 0;
    sbuf->isBind = false;
    SbufInterfaceAssign(&sbuf->infImpl);
    return sbuf;
}

struct HdfSBufImpl *SbufObtainRaw(size_t capacity)
{
    struct HdfSBufRaw *sbuf = SbufRawImplNewInstance(capacity);
    if (sbuf == NULL) {
        return NULL;
    }
    return &sbuf->infImpl;
}

struct HdfSBufImpl *SbufBindRaw(uintptr_t base, size_t size)
{
    struct HdfSBufRaw *sbuf = NULL;
    if (base == 0 || size == 0) {
        return NULL;
    }
    /* 4-byte alignment is required for base. */
    if ((base & 0x3) != 0) {
        HDF_LOGE("Base not in 4-byte alignment");
        return NULL;
    }
    sbuf = (struct HdfSBufRaw *)OsalMemAlloc(sizeof(struct HdfSBufRaw));
    if (sbuf == NULL) {
        HDF_LOGE("%s: oom", __func__);
        return NULL;
    }

    sbuf->data = (uint8_t *)base;
    sbuf->capacity = size;
    sbuf->writePos = size;
    sbuf->readPos = 0;
    sbuf->isBind = true;
    SbufInterfaceAssign(&sbuf->infImpl);
    return &sbuf->infImpl;
}