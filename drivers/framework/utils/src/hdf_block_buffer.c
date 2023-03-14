/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
 
#include "hdf_block_buffer.h"
#include "osal_mem.h"
#include "securec.h"

static const unsigned int OFFSET = 8;

struct HdfHdfBlockBuffer *HdfHdfBlockBufferNew(const uint8_t *data, uint16_t size)
{
    uint16_t wantedSize;
    struct HdfBlockBuffer *buffer = NULL;
    if (size == 0) {
        return NULL;
    }
    wantedSize = sizeof(struct HdfBlockBuffer) + size;
    buffer = (struct HdfBlockBuffer *)OsalMemAlloc(wantedSize);
    if (buffer == NULL) {
        return NULL;
    }
    buffer->dataSize = size;
    buffer->position = 0;
    if (data != NULL) {
        if (memcpy_s(buffer->data, buffer->dataSize, data, size) != EOK) {
            OsalMemFree(buffer);
            return NULL;
        }
    }
    return buffer;
}

void HdfBlockBufferFree(const struct HdfBlockBuffer *buffer)
{
    if (buffer != NULL) {
        OsalMemFree(buffer);
    }
}

uint16_t HdfBlockBufferGetDataSize(struct HdfBlockBuffer *buffer)
{
    return (buffer == NULL) ? 0 : buffer->dataSize;
}

uint16_t HdfBlockBufferGetAvailableSize(struct HdfBlockBuffer *buffer)
{
    return (buffer == NULL) ? 0 : (buffer->dataSize - buffer->position);
}

uint8_t *HdfBlockBufferRead(struct HdfBlockBuffer *buffer, uint16_t size)
{
    uint8_t *dataPtr = NULL;
    if ((buffer == NULL) || (buffer->dataSize - buffer->position < size)) {
        return NULL;
    }
    dataPtr = &buffer->data[buffer->position];
    buffer->position += size;
    return dataPtr;
}

uint8_t *HdfBlockBufferGetData(struct HdfBlockBuffer *buffer)
{
    return (buffer == NULL) ? NULL : &buffer->data[0];
}

uint16_t HdfBlockBufferGetPoistion(struct HdfBlockBuffer *buffer)
{
    return (buffer == NULL) ? 0 : buffer->position;
}


uint16_t HdfBlockBufferSkipBytes(struct HdfBlockBuffer *buffer, uint16_t bytes)
{
    if (buffer == NULL) {
        return 0;
    }
    if (buffer->position + bytes <= buffer->dataSize) {
        buffer->position += bytes;
        return buffer->dataSize - buffer->position;
    } else {
        buffer->position = buffer->dataSize;
        return 0;
    }
}

bool HdfBlockBufferReadUint8(struct HdfBlockBuffer *buffer, uint8_t *outValue)
{
    if (buffer == NULL) {
        return false;
    }
    if (buffer->position + BYTES_UINT8 <= buffer->dataSize) {
        *outValue = buffer->data[buffer->position++];
        return true;
    }
    return false;
}

bool HdfBlockBufferReadUint8At(struct HdfBlockBuffer *buffer, uint16_t idx, uint8_t *outValue)
{
    if (buffer == NULL) {
        return false;
    }
    if (idx + BYTES_UINT8 <= buffer->dataSize) {
        *outValue = buffer->data[idx];
        return true;
    }
    return false;
}

bool HdfBlockBufferReadUint16(struct HdfBlockBuffer *buffer, uint16_t *outValue)
{
    if (buffer == NULL) {
        return false;
    }
    if (buffer->position + BYTES_UINT16 <= buffer->dataSize) {
        *outValue = read_be16(buffer->data, buffer->position);
        buffer->position += BYTES_UINT16;
        return true;
    }
    return false;
}

bool HdfBlockBufferReadUint16At(struct HdfBlockBuffer *buffer, size_t idx, uint16_t *outValue)
{
    if (buffer == NULL) {
        return false;
    }
    if (idx + BYTES_UINT16 <= buffer->dataSize) {
        *outValue = read_be16(buffer->data, idx);
        return true;
    }
    return false;
}

void HdfBlockBufferRewind(struct HdfBlockBuffer *buffer)
{
    if (buffer != NULL) {
        buffer->position = 0;
    }
}

bool HdfBlockBufferWriteUint8(struct HdfBlockBuffer *buffer, uint8_t value)
{
    if (buffer == NULL) {
        return false;
    }
    if (buffer->position + sizeof(uint8_t) <= buffer->dataSize) {
        buffer->data[buffer->position++] = value;
        return true;
    }
    return false;
}

bool HdfBlockBufferWriteUint16(struct HdfBlockBuffer *buffer, uint16_t inValue)
{
    if (buffer == NULL) {
        return false;
    }
    if (buffer->position + BYTES_UINT16 <= buffer->dataSize) {
        buffer->data[buffer->position++] = (uint8_t) (inValue >> OFFSET);
        buffer->data[buffer->position++] = (uint8_t) (inValue & 0xFF);
        return true;
    }
    return false;
}

bool HdfBlockBufferWriteData(struct HdfBlockBuffer *buffer, uint8_t *data, size_t length)
{
    uint16_t residualSize;
    if (buffer == NULL) {
        return false;
    }
    residualSize = buffer->dataSize - buffer->position;
    if (memcpy_s(buffer->data + buffer->position, residualSize, data, length) != EOK) {
        return false;
    }
    buffer->position += length;
    return true;
}

struct HdfBlockBuffer *HdfBlockBufferDuplicate(const struct HdfBlockBuffer *buffer, uint16_t start, uint16_t end)
{
    uint16_t bufferSize = HdfBlockBufferGetDataSize(buffer);
    uint16_t newBufferSize;
    struct HdfBlockBuffer *newBuffer = NULL;
    if ((buffer == NULL) || (start > end)) {
        return NULL;
    }
    if ((end > bufferSize) || (start > bufferSize)) {
        return NULL;
    }
    newBufferSize = end - start + 1;
    newBuffer = HdfBlockBufferNew(newBufferSize);
    if (newBuffer == NULL) {
        return NULL;
    }
    if (memcpy_s(newBuffer->data, newBufferSize, buffer->data + start, newBufferSize) != EOK) {
        OsalMemFree(newBuffer);
        return NULL;
    }

    return newBuffer;
}

bool HdfBlockBufferWriteBuff(struct HdfBlockBuffer *dst, struct HdfBlockBuffer *src)
{
    if (memcpy_s(&dst->data[dst->position], dst->dataSize - dst->position, src->data, src->dataSize) != EOK) {
        return false;
    }
    dst->position += src->dataSize;
    return true;
}

