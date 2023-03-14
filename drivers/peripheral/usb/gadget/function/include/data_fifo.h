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

#ifndef HDF_USB_DATA_FIFO_H
#define HDF_USB_DATA_FIFO_H

#include "hdf_base.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

struct DataFifo {
    uint32_t rdIdx;
    uint32_t wrIdx;
    uint32_t size;
    void     *data;
};

/* DataFifo Helper function */
static inline void DataFifoInit(struct DataFifo *fifo, uint32_t size, void *data)
{
    fifo->rdIdx = 0;
    fifo->wrIdx = 0;
    fifo->size = size;
    fifo->data = data;
}

static inline bool DataFifoIsInitialized(struct DataFifo *fifo)
{
    return fifo->size != 0;
}

static inline void DataFifoReset(struct DataFifo *fifo)
{
    fifo->rdIdx = fifo->wrIdx = 0;
}

static inline uint32_t DataFifoLen(struct DataFifo *fifo)
{
    return fifo->wrIdx - fifo->rdIdx;
}

static inline bool DataFifoIsEmpty(struct DataFifo *fifo)
{
    return fifo->wrIdx == fifo->rdIdx;
}

static inline bool DataFifoIsFull(struct DataFifo *fifo)
{
    return DataFifoLen(fifo) > (fifo->size - 1);
}

static inline uint32_t DataFifoAvailSize(struct DataFifo *fifo)
{
    return fifo->size - DataFifoLen(fifo);
}

static inline void DataFifoSkip(struct DataFifo *fifo, uint32_t size)
{
    fifo->rdIdx += size;
}

static inline uint32_t DataFifoWrite(struct DataFifo *fifo, uint8_t *data, uint32_t size)
{
    uint32_t mask = fifo->size - 1;
    uint8_t *buf = fifo->data;
    uint8_t *end;

    size = MIN(size, DataFifoAvailSize(fifo));
    end = data + size;
    while (data < end) {
        buf[fifo->wrIdx++ & mask] = *data++;
    }
    return size;
}

static inline uint32_t DataFifoRead(struct DataFifo *fifo, uint8_t *data, uint32_t size)
{
    uint32_t mask = fifo->size - 1;
    uint8_t *buf = fifo->data;
    uint8_t *end;

    size = MIN(size, DataFifoLen(fifo));
    end = data + size;
    while (data < end) {
        *data++ = buf[fifo->rdIdx++ & mask];
    }
    return size;
}

#endif /* HDF_USB_DATA_FIFO_H */
