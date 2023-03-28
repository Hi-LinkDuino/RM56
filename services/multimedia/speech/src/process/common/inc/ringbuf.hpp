#include <stdint.h>
#include <stdbool.h>
#include "ae_common.h"
#include "ae_memory.h"

struct RingbufState
{
    uint32_t head; /* 1st element */
    uint32_t tail; /* last element plus 1 */
    uint32_t size;
    uint32_t len;
    void *buf;
};

typedef void Ringbuf_forfn(uint8_t);

template <typename DataType>
RingbufState *ringbuf_create(uint32_t size)
{
    RingbufState *st = (RingbufState *)AE_MALLOC(sizeof(struct RingbufState));

    st->buf = (void *)AE_MALLOC(size * sizeof(DataType));
    st->head = 0;
    st->tail = 0;
    st->size = size;
    st->len = 0;

    AE_MEMSET(st->buf, 0, st->size * sizeof(DataType));

    return st;
}

template <typename DataType>
void ringbuf_reset(struct RingbufState *st)
{
    AE_MEMSET(st->buf, 0, st->size * sizeof(DataType));
    st->head = 0;
    st->tail = 0;
}

template <typename DataType>
void ringbuf_delete(struct RingbufState *st)
{
    AE_FREE(st->buf);

    AE_FREE(st);
}

template <typename DataType>
uint32_t ringbuf_available(const struct RingbufState *st)
{
    return (st->size - st->len);
}

template <typename DataType>
bool ringbuf_is_empty(const struct RingbufState *st)
{
    return (st->len == 0 ? 1 : 0);
}

template <typename DataType>
int32_t ringbuf_write(struct RingbufState *st, const DataType *data, uint32_t len)
{
    if (ringbuf_available<DataType>(st) < len) {
        return -1;
    }

    DataType *buf = (DataType *)st->buf;

    if (st->size - st->tail < len) {
        uint32_t len1 = st->size - st->tail;
        //AE_MEMCPY(st->buf + st->tail, data, len1);
        for (uint32_t i = 0; i < len1; i++)
            buf[st->tail + i] = data[i];
        //AE_MEMCPY(st->buf, data + len1, len - len1);
        for (uint32_t i = 0; i < len - len1; i++)
            buf[i] = data[len1 + i];
        st->tail = st->tail + len - st->size;
    } else {
        //AE_MEMCPY(st->buf + st->tail, data, len);
        for (uint32_t i = 0; i < len; i++)
            buf[st->tail + i] = data[i];
        st->tail = st->tail + len;
    }

    st->len += len;

    return 0;
}

template <typename DataType>
int32_t ringbuf_write2(struct RingbufState *st, const DataType *data, uint32_t len, int stride)
{
    if (ringbuf_available<DataType>(st) < len) {
        return -1;
    }

    DataType *buf = (DataType *)st->buf;

    if (st->size - st->tail < len) {
        uint32_t len1 = st->size - st->tail;
        //AE_MEMCPY(st->buf + st->tail, data, len1);
        for (uint32_t i = 0, j = 0; i < len1; i++, j += stride)
            buf[st->tail + i] = data[j];
        //AE_MEMCPY(st->buf, data + len1, len - len1);
        for (uint32_t i = 0, j = len1 * stride; i < len - len1; i++, j += stride)
            buf[i] = data[j];
        st->tail = st->tail + len - st->size;
    } else {
        //AE_MEMCPY(st->buf + st->tail, data, len);
        for (uint32_t i = 0, j = 0; i < len; i++, j += stride)
            buf[st->tail + i] = data[j];
        st->tail = st->tail + len;
    }

    st->len += len;

    return 0;
}

template <typename DataType>
int32_t ringbuf_write_zeros(struct RingbufState *st, uint32_t len)
{
    if (ringbuf_available<DataType>(st) < len) {
        return -1;
    }

    DataType *buf = (DataType *)st->buf;

    if (st->size - st->tail < len) {
        uint32_t len1 = st->size - st->tail;
        AE_MEMSET(buf + st->tail, 0, len1 * sizeof(DataType));
        AE_MEMSET(buf, 0, (len - len1) * sizeof(DataType));
        st->tail = st->tail + len - st->size;
    }
    else {
        AE_MEMSET(buf + st->tail, 0, len * sizeof(DataType));
        st->tail = st->tail + len;
    }

    st->len += len;

    return 0;
}

template <typename DataType>
int32_t ringbuf_read(struct RingbufState *st, DataType *data, uint32_t len)
{
    if (st->size - ringbuf_available<DataType>(st) < len) {
        return -1;
    }

    DataType *buf = (DataType *)st->buf;

    if (st->size - st->head < len) {
        uint32_t len1 = st->size - st->head;
        //AE_MEMCPY(data, st->buf + st->head, len1);
        for (uint32_t i = 0; i < len1; i++)
            data[i] = buf[st->head + i];
        //AE_MEMCPY(data + len1, st->buf, len - len1);
        for (uint32_t i = 0; i < len - len1; i++)
            data[len1 + i] = buf[i];
        st->head = st->head + len - st->size;
    }
    else {
        //AE_MEMCPY(data, st->buf + st->head, len);
        for (uint32_t i = 0; i < len; i++)
            data[i] = buf[st->head + i];
        st->head = st->head + len;
    }

    st->len -= len;

    return 0;
}

template <typename DataType>
int32_t ringbuf_read2(struct RingbufState *st, DataType *data, uint32_t len, int stride)
{
    if (st->size - ringbuf_available<DataType>(st) < len) {
        return -1;
    }

    DataType *buf = (DataType *)st->buf;

    if (st->size - st->head < len) {
        uint32_t len1 = st->size - st->head;
        //AE_MEMCPY(data, st->buf + st->head, len1);
        for (uint32_t i = 0, j = 0; i < len1; i++, j += stride)
            data[j] = buf[st->head + i];
        //AE_MEMCPY(data + len1, st->buf, len - len1);
        for (uint32_t i = 0, j = len1 * stride; i < len - len1; i++, j += stride)
            data[j] = buf[i];
        st->head = st->head + len - st->size;
    } else {
        //AE_MEMCPY(data, st->buf + st->head, len);
        for (uint32_t i = 0, j = 0; i < len; i++, j += stride)
            data[j] = buf[st->head + i];
        st->head = st->head + len;
    }

    st->len -= len;

    return 0;
}

template <typename DataType>
int32_t ringbuf_peek(const struct RingbufState *st, uint8_t *data, uint32_t len)
{
    if (st->size - ringbuf_available<DataType>(st) < len) {
        return -1;
    }

    DataType *buf = (DataType *)st->buf;

    if (st->head > st->tail && st->size - st->head < len) {
        uint32_t len1 = st->size - st->head;
        //AE_MEMCPY(data, st->buf + st->head, len1);
        for (uint32_t i = 0; i < len1; i++)
            data[i] = buf[st->head + i];
        //AE_MEMCPY(data + len1, st->buf, len - len1);
        for (uint32_t i = 0; i < len - len1; i++)
            data[len1 + i] = buf[i];
    }
    else {
        //AE_MEMCPY(data, st->buf + st->head, len);
        for (uint32_t i = 0; i < len; i++)
            data[i] = buf[st->head + i];
    }

    return 0;
}

template <typename DataType>
void ringbuf_for_each(struct RingbufState *st, Ringbuf_forfn func)
{
    DataType *buf = (DataType *)st->buf;

    if (ringbuf_is_empty<DataType>(st) == 0) {
        uint32_t len = st->len;
        uint32_t i = st->head;
        while (len-- > 0) {
            func(buf[i++]);
            if (i == st->size)
                i = 0;
        }
    }
}

void dump(uint8_t ch)
{
    AE_TRACE(1,"%d", ch);
}

template <typename DataType>
void ringbuf_dump(const struct RingbufState *st)
{
    ringbuf_for_each<DataType>((struct RingbufState *)st, dump);
}
