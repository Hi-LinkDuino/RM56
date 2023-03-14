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

#include "buffer.h"
#include <stdlib.h>
#include <memory.h>
#include <stdatomic.h>
#include "platform/include/platform_def.h"

typedef struct Buffer {
    uint32_t size;
    atomic_uint_least32_t refcount;
    Buffer *rootbuf;
    uint8_t *data;
} BufferInternal;

Buffer *BufferMalloc(uint32_t size)
{
    if (size == 0) {
        return NULL;
    }

    Buffer *buf = (Buffer *)calloc(1, (sizeof(Buffer) + size));
    if (buf == NULL) {
        return NULL;
    }

    buf->size = size;
    buf->refcount = 1;
    buf->rootbuf = buf;
    buf->data = (uint8_t *)buf + sizeof(Buffer);
    return buf;
}

Buffer *BufferRefMalloc(const Buffer *buf)
{
    if (buf == NULL) {
        return NULL;
    }

    Buffer *ref = (Buffer *)calloc(1, sizeof(Buffer));
    if (ref == NULL) {
        return NULL;
    }

    ref->data = buf->data;
    ref->size = buf->size;
    ref->rootbuf = buf->rootbuf;
    atomic_fetch_add_explicit(&ref->rootbuf->refcount, 1, memory_order_seq_cst);

    return ref;
}

Buffer *BufferSliceMalloc(const Buffer *buf, uint32_t offset, uint32_t size)
{
    if (buf == NULL) {
        return NULL;
    }

    if ((size + offset > buf->size) || (size == 0)) {
        LOG_ERROR("Size too small or too big");
        return NULL;
    }

    if (offset > buf->size) {
        LOG_ERROR("BufferSliceMalloc##Buffer offset out of bound");
        return NULL;
    }

    Buffer *slice = BufferRefMalloc(buf);
    slice = BufferResize(slice, offset, size);

    return slice;
}

Buffer *BufferResize(Buffer *buf, uint32_t offset, uint32_t size)
{
    if (buf == NULL) {
        return NULL;
    }
    if (size + offset > buf->size) {
        LOG_ERROR("Size too small or too big");
        return buf;
    }

    if (buf->rootbuf == buf) {
        Buffer *ref = BufferRefMalloc(buf);
        atomic_fetch_add_explicit(&ref->rootbuf->refcount, -1, memory_order_seq_cst);
        ref->data += offset;
        ref->size = size;
        return ref;
    } else {
        buf->data += offset;
        buf->size = size;
        return buf;
    }
}

void BufferFree(Buffer *buf)
{
    if (buf == NULL) {
        return;
    }

    if (buf->rootbuf != buf) {
        if (atomic_fetch_add_explicit(&buf->rootbuf->refcount, -1, memory_order_seq_cst) == 1) {
            free(buf->rootbuf);
        }
        free(buf);
    } else if (atomic_fetch_add_explicit(&buf->refcount, -1, memory_order_seq_cst) == 1) {
        free(buf->rootbuf);
    }
}

void *BufferPtr(const Buffer *buf)
{
    if (buf == NULL) {
        return NULL;
    }

    return buf->data;
}

uint32_t BufferGetSize(const Buffer *buf)
{
    if (buf == NULL) {
        return 0;
    }

    return buf->size;
}