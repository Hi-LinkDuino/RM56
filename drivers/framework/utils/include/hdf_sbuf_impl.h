/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_SBU_IMPL_H
#define HDF_SBU_IMPL_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfSbufConstructor {
    struct HdfSBufImpl *(*obtain)(size_t capacity);
    struct HdfSBufImpl *(*bind)(uintptr_t base, size_t size);
};

struct HdfRemoteService;

struct HdfSBufImpl {
    bool (*writeBuffer)(struct HdfSBufImpl *sbuf, const uint8_t *data, uint32_t writeSize);
    bool (*writeUnpadBuffer)(struct HdfSBufImpl *sbuf, const uint8_t *data, uint32_t writeSize);
    bool (*writeUint64)(struct HdfSBufImpl *sbuf, uint64_t value);
    bool (*writeUint32)(struct HdfSBufImpl *sbuf, uint32_t value);
    bool (*writeUint16)(struct HdfSBufImpl *sbuf, uint16_t value);
    bool (*writeUint8)(struct HdfSBufImpl *sbuf, uint8_t value);
    bool (*writeInt64)(struct HdfSBufImpl *sbuf, int64_t value);
    bool (*writeInt32)(struct HdfSBufImpl *sbuf, int32_t value);
    bool (*writeInt16)(struct HdfSBufImpl *sbuf, int16_t value);
    bool (*writeInt8)(struct HdfSBufImpl *sbuf, int8_t value);
    bool (*writeString)(struct HdfSBufImpl *sbuf, const char *value);
    bool (*writeFileDescriptor)(struct HdfSBufImpl *sbuf, int fd);
    bool (*writeFloat)(struct HdfSBufImpl *sbuf, float value);
    bool (*writeDouble)(struct HdfSBufImpl *sbuf, double value);
    bool (*readDouble)(struct HdfSBufImpl *sbuf, double *value);
    bool (*readFloat)(struct HdfSBufImpl *sbuf, float *value);
    int (*readFileDescriptor)(struct HdfSBufImpl *sbuf);
    bool (*writeString16)(struct HdfSBufImpl *sbuf, const char16_t *value, uint32_t size);
    bool (*readBuffer)(struct HdfSBufImpl *sbuf, const uint8_t **data, uint32_t *readSize);
    const uint8_t *(*readUnpadBuffer)(struct HdfSBufImpl *sbuf, size_t length);
    bool (*readUint64)(struct HdfSBufImpl *sbuf, uint64_t *value);
    bool (*readUint32)(struct HdfSBufImpl *sbuf, uint32_t *value);
    bool (*readUint16)(struct HdfSBufImpl *sbuf, uint16_t *value);
    bool (*readUint8)(struct HdfSBufImpl *sbuf, uint8_t *value);
    bool (*readInt64)(struct HdfSBufImpl *sbuf, int64_t *value);
    bool (*readInt32)(struct HdfSBufImpl *sbuf, int32_t *value);
    bool (*readInt16)(struct HdfSBufImpl *sbuf, int16_t *value);
    bool (*readInt8)(struct HdfSBufImpl *sbuf, int8_t *value);
    const char *(*readString)(struct HdfSBufImpl *sbuf);
    const char16_t *(*readString16)(struct HdfSBufImpl *sbuf);
    int32_t (*writeRemoteService)(struct HdfSBufImpl *sbuf, const struct HdfRemoteService *service);
    struct HdfRemoteService *(*readRemoteService)(struct HdfSBufImpl *sbuf);
    const uint8_t *(*getData)(const struct HdfSBufImpl *sbuf);
    void (*flush)(struct HdfSBufImpl *sbuf);
    size_t (*getCapacity)(const struct HdfSBufImpl *sbuf);
    size_t (*getDataSize)(const struct HdfSBufImpl *sbuf);
    void (*setDataSize)(struct HdfSBufImpl *sbuf, size_t size);
    void (*recycle)(struct HdfSBufImpl *sbuf);
    struct HdfSBufImpl *(*move)(struct HdfSBufImpl *sbuf);
    struct HdfSBufImpl *(*copy)(const struct HdfSBufImpl *sbuf);
    void (*transDataOwnership)(struct HdfSBufImpl *sbuf);
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_SBUF_H */
/** @} */
