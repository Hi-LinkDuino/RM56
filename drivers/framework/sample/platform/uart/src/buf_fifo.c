/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "buf_fifo.h"

bool BufferFifoInit(struct BufferFifo *fifo, uint8_t *fifoBuffer, uint16_t fifoSize)
{
    if (fifoBuffer == NULL) {
        return false;
    }
    if (!IsPowerOfTwo(fifoSize)) {
        return false;
    }
    fifo->buffer = fifoBuffer;
    fifo->bufSizeMask = fifoSize - 1;
    fifo->readPosition = 0;
    fifo->writePosition = 0;
    return true;
}