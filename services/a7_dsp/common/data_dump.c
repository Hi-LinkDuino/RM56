#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kfifo/kfifo.h"
#include "data_dump.h"
#ifdef RTOS
#include "cmsis_os.h"
#include "bes_kv/bes_kv.h"
#else
#include "net_memory.h"
#endif
#include "hal_trace.h"
#include "hal_timer.h"
#include "mcu_audio.h"


#define _MULTI_DATA_DUMP_NUM 6

static multi_data_dump_t data_dump_array[_MULTI_DATA_DUMP_NUM];

static void __data_dump_input(unsigned char *data, unsigned int len, void *param)
{
    multi_data_dump_t *_hdl = (multi_data_dump_t *)param;
    struct kfifo *fifo = (struct kfifo *)_hdl->param;
    uint8_t *_buf = data;
    int _len = len;
    int w_len = 0;
    if (_hdl->state == DATA_DUMP_START)
    {
#ifdef RTOS
        uint32_t _tick = TICKS_TO_MS(hal_sys_timer_get());
        osMutexWait(_hdl->mutex_id, osWaitForever);
        _tick = TICKS_TO_MS(hal_sys_timer_get()) - _tick;
        if (_tick > 5) {
            printf("Irq %d-%d Wait%dms ", _hdl->idx, len, _tick);
        }
#endif
        if (_hdl->input_cb) {
            _hdl->input_cb(param, data, len, _hdl->samplerate, _hdl->ch_num, &_buf, &_len);
        }
        if (kfifo_available(fifo) >= _len) {
            w_len = kfifo_put(fifo, _buf, _len);
        }
        int cur_len = kfifo_len(fifo);
        if (_hdl->max_buf_size < cur_len) {
            _hdl->max_buf_size = cur_len;
        }
#ifdef RTOS
        osMutexRelease(_hdl->mutex_id);
#endif
        if (w_len == _len)
        {
            // printf("&%d-%d ", _hdl->idx, _len);
#ifdef RTOS
            bes_kv_item_set(_hdl->calc_input_data, _len);
#endif
        }
        else
        {
#ifdef RTOS
            int total_lost = bes_kv_item_set(_hdl->calc_lost_data, _len);
            printf("idx%d overrun drop %d[total %d]\n", _hdl->idx, _len, total_lost);
#else
            printf("idx%d overrun drop %d\n", _hdl->idx, _len);
#endif
        }
    }
}

void *data_dump_open(data_dump_cb_register_t register_cbf, unsigned int kfifo_len_KB, int samplerate, int auto_start)
{
    multi_data_dump_t *_hdl = NULL;

    if ((samplerate != 16000) && (samplerate != 48000))
    {
        printf("%s samplerate=%d error! only support 16000&48000\n", __FUNCTION__, samplerate);
        return NULL;
    }

    for (int i = 0; i < _MULTI_DATA_DUMP_NUM; i++)
    {
        if (!data_dump_array[i].handle)
        {
            _hdl = &data_dump_array[i];
            _hdl->idx = i + 1;
            _hdl->handle = (void *)0x12345678;
            break;
        }
    }
    if (!_hdl)
    {
        printf("%s fail since no more handles!\n", __FUNCTION__);
        return NULL;
    }
#ifdef RTOS
#if defined(KERNEL_RTX)
    _hdl->mutex_def.mutex = (kmutex_t *)_hdl->os_mutex_cb;
#elif defined(KERNEL_RTX5)
    memset(&_hdl->mutex_def, 0, sizeof(osMutexDef_t));
    _hdl->mutex_def.attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust;
#elif defined(FREERTOS)
    memset(&_hdl->mutex_def, 0, sizeof(osMutexDef_t));
    _hdl->mutex_def.attr_bits = osMutexRecursive | osMutexPrioInherit;
#endif
    _hdl->mutex_id = osMutexCreate(&_hdl->mutex_def);

    char calc_tag[MAX_NEME_LEN];
    snprintf(calc_tag, MAX_NEME_LEN, "datadump%d_input", _hdl->idx);
    _hdl->calc_input_data = bes_kv_add(calc_tag);
    snprintf(calc_tag, MAX_NEME_LEN, "datadump%d_output", _hdl->idx);
    _hdl->calc_output_data = bes_kv_add(calc_tag);
    snprintf(calc_tag, MAX_NEME_LEN, "datadump%d_lost", _hdl->idx);
    _hdl->calc_lost_data = bes_kv_add(calc_tag);
#endif
    _hdl->state = DATA_DUMP_OPEN;
    _hdl->samplerate = samplerate;

    // align to upper 2^n since kfifo required
    unsigned int _kfifo_len = kfifo_len_KB;
    unsigned int mask = 0x80000000;
    while (mask) {
        if (mask & _kfifo_len) {
            if (mask != _kfifo_len) {
                mask <<= 1;
            }
            break;
        }
        mask >>= 1;
    }
    _kfifo_len = mask*1024;

    struct kfifo *fifo = NULL;
#ifdef RTOS
    fifo = malloc(sizeof(struct kfifo));
    if (fifo == NULL) {
        printf("%s malloc fifo fail!\n", __FUNCTION__);
        return NULL;
    }
#endif
    _hdl->param = fifo;
    _hdl->max_buf_size = 0;
    void *_kfifo_buf = NULL;
#ifdef RTOS
    _kfifo_buf = malloc(_kfifo_len);
#endif
    if (!_kfifo_buf)
    {
        printf("%s fail since no memory for kfifo!\n", __FUNCTION__);
        free(fifo);
        return NULL;
    }
    kfifo_init(fifo, _kfifo_buf, _kfifo_len);
    register_cbf(__data_dump_input, _hdl);

    if (auto_start != 0) {
        _hdl->state = DATA_DUMP_START;
    }

    printf("%s handle=%p idx=%d samplerate=%d[MIC_BITRATE=%d] param=%p, _kfifo_len=%d\n", __FUNCTION__, _hdl, _hdl->idx, _hdl->samplerate, MIC_BITRATE, _hdl->param, _kfifo_len);
    return (void *)_hdl;
}

void *data_dump_open_withfifo(data_dump_cb_register_t register_cbf, void *val_fifo, int samplerate, int auto_start)
{
    multi_data_dump_t *_hdl = NULL;

    if ((samplerate != 16000) && (samplerate != 48000))
    {
        printf("%s samplerate=%d error! only support 16000&48000\n", __FUNCTION__, samplerate);
        return NULL;
    }

    for (int i = 0; i < _MULTI_DATA_DUMP_NUM; i++)
    {
        if (!data_dump_array[i].handle)
        {
            _hdl = &data_dump_array[i];
            _hdl->idx = i + 1;
            _hdl->handle = (void *)0x12345678;
            break;
        }
    }
    if (!_hdl)
    {
        printf("%s fail since no more handles!\n", __FUNCTION__);
        return NULL;
    }
#ifdef RTOS
#if defined(KERNEL_RTX)
    _hdl->mutex_def.mutex = (kmutex_t *)_hdl->os_mutex_cb;
#elif defined(KERNEL_RTX5)
    memset(&_hdl->mutex_def, 0, sizeof(osMutexDef_t));
    _hdl->mutex_def.attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust;
#elif defined(FREERTOS)
    memset(&_hdl->mutex_def, 0, sizeof(osMutexDef_t));
    _hdl->mutex_def.attr_bits = osMutexRecursive | osMutexPrioInherit;
#endif
    _hdl->mutex_id = osMutexCreate(&_hdl->mutex_def);

    char calc_tag[MAX_NEME_LEN];
    snprintf(calc_tag, MAX_NEME_LEN, "datadump%d_input", _hdl->idx);
    _hdl->calc_input_data = bes_kv_add(calc_tag);
    snprintf(calc_tag, MAX_NEME_LEN, "datadump%d_output", _hdl->idx);
    _hdl->calc_output_data = bes_kv_add(calc_tag);
    snprintf(calc_tag, MAX_NEME_LEN, "datadump%d_lost", _hdl->idx);
    _hdl->calc_lost_data = bes_kv_add(calc_tag);
#endif
    _hdl->state = DATA_DUMP_OPEN;
    _hdl->samplerate = samplerate;

    _hdl->param = val_fifo;
    _hdl->max_buf_size = 0;

    register_cbf(__data_dump_input, _hdl);

    if (auto_start != 0) {
        _hdl->state = DATA_DUMP_START;
    }

    //printf("%s handle=%p idx=%d samplerate=%d[MIC_BITRATE=%d] param=%p, _kfifo_len=%d\n", __FUNCTION__, _hdl, _hdl->idx, _hdl->samplerate, MIC_BITRATE, _hdl->param, _kfifo_len);
    return (void *)_hdl;
}

void *data_dump_open_withcb(data_dump_cb_register_t register_cbf, data_dump_input_cb_t input_cb, unsigned int kfifo_len_KB, int samplerate, int ch_num, int auto_start)
{
    multi_data_dump_t *_hdl = NULL;

    if ((samplerate != 16000) && (samplerate != 48000))
    {
        printf("%s samplerate=%d error! only support 16000&48000\n", __FUNCTION__, samplerate);
        return NULL;
    }

    for (int i = 0; i < _MULTI_DATA_DUMP_NUM; i++)
    {
        if (!data_dump_array[i].handle)
        {
            _hdl = &data_dump_array[i];
            _hdl->idx = i + 1;
            _hdl->handle = (void *)0x12345678;
            break;
        }
    }
    if (!_hdl)
    {
        printf("%s fail since no more handles!\n", __FUNCTION__);
        return NULL;
    }
#ifdef RTOS
#if defined(KERNEL_RTX)
    _hdl->mutex_def.mutex = (kmutex_t *)_hdl->os_mutex_cb;
#elif defined(KERNEL_RTX5)
    memset(&_hdl->mutex_def, 0, sizeof(osMutexDef_t));
    _hdl->mutex_def.attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust;
#elif defined(FREERTOS)
    memset(&_hdl->mutex_def, 0, sizeof(osMutexDef_t));
    _hdl->mutex_def.attr_bits = osMutexRecursive | osMutexPrioInherit;
#endif
    _hdl->mutex_id = osMutexCreate(&_hdl->mutex_def);

    char calc_tag[MAX_NEME_LEN];
    snprintf(calc_tag, MAX_NEME_LEN, "datadump%d_input", _hdl->idx);
    _hdl->calc_input_data = bes_kv_add(calc_tag);
    snprintf(calc_tag, MAX_NEME_LEN, "datadump%d_output", _hdl->idx);
    _hdl->calc_output_data = bes_kv_add(calc_tag);
    snprintf(calc_tag, MAX_NEME_LEN, "datadump%d_lost", _hdl->idx);
    _hdl->calc_lost_data = bes_kv_add(calc_tag);
#endif
    _hdl->state = DATA_DUMP_OPEN;
    _hdl->samplerate = samplerate;
    _hdl->input_cb = input_cb;
    _hdl->ch_num = ch_num;

    // align to upper 2^n since kfifo required
    unsigned int _kfifo_len = kfifo_len_KB;
    unsigned int mask = 0x80000000;
    while (mask) {
        if (mask & _kfifo_len) {
            if (mask != _kfifo_len) {
                mask <<= 1;
            }
            break;
        }
        mask >>= 1;
    }
    _kfifo_len = mask*1024;

    struct kfifo *fifo = NULL;
#ifdef RTOS
    fifo = malloc(sizeof(struct kfifo));
    if (fifo == NULL) {
        printf("%s malloc kfifo fail!\n", __FUNCTION__);
        return NULL;
    }
#endif
    _hdl->param = fifo;
    _hdl->max_buf_size = 0;
    void *_kfifo_buf = NULL;
#ifdef RTOS
    _kfifo_buf = malloc(_kfifo_len);
#endif
    if (!_kfifo_buf)
    {
        printf("%s fail since no memory for kfifo!\n", __FUNCTION__);
        free(fifo);
        return NULL;
    }
    kfifo_init(fifo, _kfifo_buf, _kfifo_len);
    register_cbf(__data_dump_input, _hdl);

    if (auto_start != 0) {
        _hdl->state = DATA_DUMP_START;
    }

    printf("%s handle=%p idx=%d samplerate=%d[MIC_BITRATE=%d], ch_num=%d param=%p, _kfifo_len=%d\n", __FUNCTION__, _hdl, _hdl->idx, _hdl->samplerate, MIC_BITRATE, _hdl->ch_num, _hdl->param, _kfifo_len);
    return (void *)_hdl;
}

int data_dump_set_timeout(void *handle, int timeout)
{
    if (handle == NULL) {
        return -1;
    }
    multi_data_dump_t *_hdl = (multi_data_dump_t *)handle;
    _hdl->timeout = timeout;
    printf("%s handle=%p idx=%d timeout=%dms\n", __FUNCTION__, _hdl, _hdl->idx, _hdl->timeout);
    return 0;
}

int data_dump_start(void *handle)
{
    if (handle == NULL) {
        TRACE(0, "%s handle is NULL!", __FUNCTION__);
        return -1;
    }
    multi_data_dump_t *_hdl = (multi_data_dump_t *)handle;
    _hdl->state = DATA_DUMP_START;
    return 0;
}

int data_dump_reset(void *handle)
{
    multi_data_dump_t *_hdl = (multi_data_dump_t *)handle;
    struct kfifo *fifo = (struct kfifo *)_hdl->param;
    TRACE(0, "%s handle=%p idx=%d\n", __FUNCTION__, _hdl, _hdl->idx);
    kfifo_init(fifo, fifo->buffer, fifo->size);
    return 0;
}

unsigned int data_dump_get_size(void *handle)
{
    unsigned int ret = 0;
    if (handle == NULL) {
        TRACE(0, "%s handle is NULL!", __FUNCTION__);
        return 0;
    }
    multi_data_dump_t *_hdl = (multi_data_dump_t *)handle;
    struct kfifo *fifo = (struct kfifo *)_hdl->param;
    if (_hdl->state == DATA_DUMP_START)
    {
        ret = kfifo_len(fifo);
    }
    return ret;
}

int data_dump_write(void *handle, void *buf, unsigned int size)
{
    __data_dump_input(buf, size, handle);
    return size;
}

int data_dump_read(void *handle, void *buf, unsigned int size)
{
    int ret = size;
    if ((handle == NULL) || (buf == NULL)) {
        TRACE(0, "%s handle(%p) or buf is NULL!", __FUNCTION__, handle);
        return 0;
    }
    multi_data_dump_t *_hdl = (multi_data_dump_t *)handle;
    struct kfifo *fifo = (struct kfifo *)_hdl->param;
    uint32_t _start = TICKS_TO_MS(hal_sys_timer_get());
    unsigned int _len = 0;
    if (_hdl->state == DATA_DUMP_START)
    {
        do
        {
#ifdef RTOS
            uint32_t _tick = TICKS_TO_MS(hal_sys_timer_get());
            osMutexWait(_hdl->mutex_id, osWaitForever);
            _tick = TICKS_TO_MS(hal_sys_timer_get()) - _tick;
            if (_tick > 5) {
                printf("R%d-%d Wait%dms ", _hdl->idx, size, _tick);
            }
#endif
            _len = kfifo_len(fifo);
            if (_len >= size)
            {
                _len = kfifo_get(fifo, buf, size);
#ifdef RTOS
                osMutexRelease(_hdl->mutex_id);
#endif
                if (_len == size)
                {
                    // printf("r%d-%d ", _hdl->idx, _len);
#ifdef RTOS
                    bes_kv_item_set(_hdl->calc_output_data, size);
#endif
                }
                else
                {
                    printf("%s:%d idx%d TBD!", __FUNCTION__, __LINE__, _hdl->idx);
                }
                break;
            }
            else
            {
#ifdef RTOS
                osMutexRelease(_hdl->mutex_id);
#endif
                if ((_hdl->timeout > 0) && (TICKS_TO_MS(hal_sys_timer_get()) - _start > _hdl->timeout)) {
                    //printf("%s:%d idx%d timeout[%dms]!", __FUNCTION__, __LINE__, _hdl->idx, _hdl->timeout);
                    return -1;
                }
                osDelay(10);
                //printf("\n%s idx%d underrun kfifo_len=%d, size=%d\n", __FUNCTION__, _hdl->idx, _len, size);
            }
        } while (1);
    }
    return ret;
}

int data_dump_stop(void *handle)
{
    int ret = 0;
    if (handle == NULL) {
        TRACE(0, "%s handle is NULL!", __FUNCTION__);
        return -1;
    }
    multi_data_dump_t *_hdl = (multi_data_dump_t *)handle;
    _hdl->state = DATA_DUMP_STOP;
    return ret;
}

void data_dump_close(void *handle, data_dump_cb_unregister_t unregister_cbf)
{
    multi_data_dump_t *_hdl = (multi_data_dump_t *)handle;
    if (_hdl == NULL) {
        TRACE(0, "%s handle is NULL!", __FUNCTION__);
        return;
    }
    TRACE(0, "%s handle=%p idx=%d max_buf_len=%d\n", __FUNCTION__, _hdl, _hdl->idx, _hdl->max_buf_size);
#ifdef RTOS
    bes_kv_item_delete(_hdl->calc_input_data);
    bes_kv_item_delete(_hdl->calc_output_data);
    bes_kv_item_delete(_hdl->calc_lost_data);
#endif
    _hdl->state = DATA_DUMP_CLOSE;
    unregister_cbf(__data_dump_input, _hdl);
    struct kfifo *fifo = (struct kfifo *)_hdl->param;
#ifdef RTOS
    osMutexWait(_hdl->mutex_id, osWaitForever);
    if (fifo) {
        if (fifo->buffer) {
            free(fifo->buffer);
            fifo->buffer = NULL;
        }
        free(fifo);
        fifo = NULL;
    }
    osMutexRelease(_hdl->mutex_id);
    osMutexDelete(_hdl->mutex_id);
#endif
    memset(_hdl, 0, sizeof(multi_data_dump_t));
}

