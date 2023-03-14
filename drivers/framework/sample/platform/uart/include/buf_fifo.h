/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef BUF_FIFO_H
#define BUF_FIFO_H

#include <stdint.h>
#include <stdbool.h>

struct BufferFifo {
    volatile uint32_t readPosition;
    volatile uint32_t writePosition;
    uint16_t bufSizeMask;
    uint8_t *buffer;
};

static inline uint16_t BufferFifoGetDataSize(struct BufferFifo *fifo)
{
    return (fifo->writePosition - fifo->readPosition);
}

static inline bool IsPowerOfTwo(int num)
{
    return (num > 0) && (num & (num - 1)) == 0;
}

bool BufferFifoInit(struct BufferFifo *fifo, uint8_t *buf, uint16_t bufSize);

#endif // BUF_FIFO_H

