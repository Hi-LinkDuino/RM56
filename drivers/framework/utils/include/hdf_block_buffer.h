/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef BLOCK_BUFFER_H
#define BLOCK_BUFFER_H

#include <stdio.h>
#include "hdf_slist.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HdfBlockBuffer {
    struct HdfSList link;
    uint16_t position;
    uint16_t dataSize;
    uint8_t data[1];
};

struct HdfBlockBuffer *HdfBlockBufferNew(uint16_t size);

void HdfBlockBufferFree(const struct BlockBuffer *buffer);

uint16_t HdfBlockBufferGetDataSize(struct BlockBuffer *buffer);

uint16_t HdfBlockBufferGetAvailableSize(struct BlockBuffer *buffer);

uint16_t HdfBlockBufferSkipBytes(struct BlockBuffer *buffer, uint16_t bytes);

uint8_t *HdfBlockBufferRead(struct BlockBuffer *buffer, uint16_t size);

/*
 * @brief read a byte value from buffer and move cursor to next.
 *
 * @param[in] buff  the instance of gdk buffer.
 * @param[in] ulOffset the offset from current cursor.
 *
 * @return  the byte value that read from buffer.
 */
bool HdfBlockBufferGetc(struct BlockBuffer *buffer, uint8_t *outValue);

/*
 * @brief read a byte value from buffer and move cursor to next.
 *
 * @param[in] buff the instance of gdk buffer.
 * @param[in] ulOffset the offset from current cursor.
 *
 * @return  the byte value that read from buffer.
 */
bool HdfBlockBufferReadByteAt(struct BlockBuffer *buffer, uint16_t idx, uint8_t *outValue);

/*
 * @brief read a short value from buffer and move cursor to next.
 *
 * @param[in] buff the instance of gdk buffer.
 * @param[in] ulOffset the offset from current cursor.
 *
 * @return the short value that read from buffer.
 */
bool HdfBlockBufferReadShort(struct BlockBuffer *buffer, uint16_t *value16);

/*
 * @brief read a short value from buffer and move cursor to next.
 *
 * @param[in] buff the instance of gdk buffer.
 * @param[in] ulOffset the offset from current cursor.
 *
 * @return the short value that read from buffer.
 */
bool HdfBlockBufferReadShortAt(struct BlockBuffer *buffer, size_t idx, uint16_t *outValue);

uint8_t *HdfBlockBufferGetData(struct BlockBuffer *buffer);

/*
 * @brief put a short value into buffer order with big endian.
 *
 * @param[in] buff the instance of gdk buffer.
 * @param[in] short_value the short value that insert into buffer..
 *
 * @return true if uint8 was write successfully, otherwise false.
 */
bool HdfBlockBufferWriteShort(struct BlockBuffer *buffer, uint16_t shortValue);

/*
 * @brief get buffer space size.
 *
 * @param[in] buff the instance of gdk buffer.
 * @param[in] start the start index of duplicate buffer rang.
 * @param[in] end the end index of duplicate buffer rang.
 *
 * @return the new instance of buff which contain specific packet.
 */
struct HdfBlockBuffer *HdfBlockBufferDuplicate(const struct HdfBlockBuffer *buffer, uint16_t start, uint16_t end);

/*
 * @brief append an byte array packet into buffer.
 *
 * @param[in] buff the instance of gdk buffer.
 * @param[in] data the start address of byte array.
 * @param[in] length the length of byte array.
 *
 * @return void
 */
void HdfBlockBufferRewind(struct HdfBlockBuffer *buffer);

/*
 * @brief put a byte into buffer.
 *
 * @param[in] buff -the instance of gdk buffer.
 * @param[in] value -the byte value that insert into buffer..
 *
 * @return true if uint8 was write successfully, otherwise false.
 */
bool HdfBlockBufferPutc(struct HdfBlockBuffer *buffer, uint8_t value);

/*
 * @brief append an byte array packet into buffer.
 *
 * @param[in] buff the instance of gdk buffer.
 * @param[in] data the start address of byte array.
 * @param[in] length the length of byte array.
 *
 * @return true if operation is success, otherwise false.
 */
bool HdfBlockBufferWriteData(struct HdfBlockBuffer *buffer, uint8_t *data, size_t length);

bool HdfBlockBufferWriteBuff(struct HdfBlockBuffer *dst, struct HdfBlockBuffer *src);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BLOCK_BUFFER_H */
