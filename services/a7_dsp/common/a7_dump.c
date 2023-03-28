#include <stdbool.h>
#include "stdio.h"
#include "string.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_location.h"
#include "transq_msg.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "data_dump.h"
#include "mcu_audio.h"
#ifdef __ARM_ARCH_ISA_ARM
#define AUD_DUMP_DATA_LEN (1024*3)
#ifdef A7_ALLOC_TRANSQ_BUF
static SYNC_FLAGS_LOC TRANSQ_MSG_T transq_msg;//must be uncached static parameter
static SYNC_FLAGS_LOC unsigned char aud_dump_data[AUD_DUMP_DATA_LEN];//must be uncached static parameter
static TRANSQ_MSG_T *p_transq_msg = &transq_msg;
static unsigned char *p_aud_dump_data = aud_dump_data;
#else
static TRANSQ_MSG_T *p_transq_msg = NULL;
static unsigned char *p_aud_dump_data = NULL;
#endif
#else
//static SYNC_FLAGS_LOC TRANSQ_MSG_T transq_msg;//must be uncached static parameter
#define AUD_DUMP_DATA_LEN (1024*3)
static SYNC_FLAGS_LOC unsigned char aud_dump_data[AUD_DUMP_DATA_LEN];//must be uncached static parameter
#endif

#ifdef RTOS
static osMutexId aud_dump_mutex_id = NULL;
osMutexDef(aud_dump_mutex);
#endif

static multi_audio_dump_t audio_dump[AUD_DUMP_SRC_NUM];

void aud_dump_cb_register(aud_dump_cb_t cbf, void *param)
{
#ifdef RTOS
    osMutexWait(aud_dump_mutex_id, osWaitForever);
#endif
    for (int i=0; i<AUD_DUMP_SRC_NUM; i++) {
        if ((!audio_dump[i].func) && (audio_dump[i].handle == AUD_DUMP_INIT)) {
            audio_dump[i].func = cbf;
            audio_dump[i].param = param;
            audio_dump[i].handle = AUD_DUMP_READY;
            printf("%s i=%d cb=%p param=%p.\r\n",__FUNCTION__,i,audio_dump[i].func,audio_dump[i].param);
            break;
        }
    }
#ifdef RTOS
    osMutexRelease(aud_dump_mutex_id);
#endif
}

void aud_dump_cb_unregister(aud_dump_cb_t cbf, void *param)
{
#ifdef RTOS
    osMutexWait(aud_dump_mutex_id, osWaitForever);
#endif
    for (int i=0; i<AUD_DUMP_SRC_NUM; i++) {
        if ((audio_dump[i].func == cbf) && (audio_dump[i].param == param)) {
            printf("%s i=%d cb=%p param=%p.\r\n",__FUNCTION__,i,audio_dump[i].func,audio_dump[i].param);
            audio_dump[i].func = NULL;
            audio_dump[i].param = NULL;
            break;
        }
    }
#ifdef RTOS
    osMutexRelease(aud_dump_mutex_id);
#endif
}

#ifdef __ARM_ARCH_ISA_ARM
void ai_record(unsigned int dump_src, unsigned char *data, unsigned int len)
{
    if (dump_src >= AUD_DUMP_SRC_NUM) {
        printf("%s %d dump_src=%d overflow [max %d]\n",__func__,__LINE__,dump_src,AUD_DUMP_SRC_NUM);
        return;
    }
#ifdef RTOS
    osMutexWait(aud_dump_mutex_id, osWaitForever);
#endif
    multi_audio_dump_t *_hdl = &audio_dump[dump_src];
    if (_hdl->handle == AUD_DUMP_READY) {
        _hdl->handle = AUD_DUMP_START;
    } else if ((_hdl->handle == AUD_DUMP_START) && (_hdl->func)) {
        //printf("%s dump_src=%d, data=0x%X, len=%d\n", __FUNCTION__, dump_src, data, len);
        _hdl->func(data, len, _hdl->param);
    }
#ifdef RTOS
    osMutexRelease(aud_dump_mutex_id);
#endif
}

#ifndef RTOS
static int transq_tx_done = 0;
static void set_transq_tx_done(void *param)
{
    transq_tx_done = 1;
}
#else
osSemaphoreDef(aud_dump_transq_tx_sem);
static osSemaphoreId transq_tx_sem = NULL;
static void set_transq_tx_done(void *param)
{
    osSemaphoreRelease(transq_tx_sem);
}
#endif

int aud_dump_send(void)
{
    int len = 0;
    int ret = 0;
    multi_audio_dump_t *_hdl = NULL;

#ifdef RTOS
    osMutexWait(aud_dump_mutex_id, osWaitForever);
#endif
    for (int i=0; i<AUD_DUMP_SRC_NUM; i++) {
        _hdl = &audio_dump[i];
        if ((_hdl->handle == AUD_DUMP_START) && (_hdl->func) && (data_dump_get_size(_hdl->param) >= AUD_DUMP_DATA_LEN)) {
            len = data_dump_read(_hdl->param, p_aud_dump_data, AUD_DUMP_DATA_LEN);
            if (len == AUD_DUMP_DATA_LEN) {
                memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
                p_transq_msg->type = TRANSQ_MSG_TYPE_AUDIO_DUMP;
                p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
                p_transq_msg->id = i;
                p_transq_msg->user_data = p_aud_dump_data;
                p_transq_msg->user_data_len = AUD_DUMP_DATA_LEN;
                transq_msg_tx_wait_done(p_transq_msg);
                ret += len;
                //printf("%s i=%d len=%d end.\r\n",__FUNCTION__,i,len);
            }
        }
    }
#ifdef RTOS
    osMutexRelease(aud_dump_mutex_id);
#endif
    return ret;
}

#else

static void aud_dump_handler(void *param)
{
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    if (msg->type != TRANSQ_MSG_TYPE_AUDIO_DUMP) {
        TRACE(0, "%s type=%d, should be TRANSQ_MSG_TYPE_AUDIO_DUMP!", __FUNCTION__, msg->type);
        return;
    }

    osMutexWait(aud_dump_mutex_id, osWaitForever);
    if (audio_dump[msg->id].func) {
        //printf("%s src=%d, data=0x%X, len=%d\n", __FUNCTION__, msg->id, msg->user_data, msg->user_data_len);
        if (audio_dump[msg->id].handle == AUD_DUMP_MIC12) {
            unsigned char *buf = aud_dump_data;
            unsigned char *_buf = msg->user_data;
            int len = msg->user_data_len;
            int r=0, w=0;
            while (r < len) {
                buf[w++] = _buf[r+CHAN_CAPTURE_MIC1*2];
                buf[w++] = _buf[r+CHAN_CAPTURE_MIC1*2+1];
                buf[w++] = _buf[r+CHAN_CAPTURE_MIC2*2];
                buf[w++] = _buf[r+CHAN_CAPTURE_MIC2*2+1];
                r += CHAN_NUM_CAPTURE*2;
            }
            len = w;
            audio_dump[msg->id].func(buf, len, audio_dump[msg->id].param);
        } else {
            audio_dump[msg->id].func(msg->user_data, msg->user_data_len, audio_dump[msg->id].param);
        }
    }
    osMutexRelease(aud_dump_mutex_id);
}

int aud_dump_set_channel(AUD_DUMP_SRC_T dump_src, int ch_map)
{
    //TRACE(0, "%s dump_src=%d, onoff=%d", __FUNCTION__, dump_src, onoff);
    if (dump_src >= AUD_DUMP_SRC_NUM) {
        printf("%s %d dump_src=%d overflow [max %d]\n",__func__,__LINE__,dump_src,AUD_DUMP_SRC_NUM);
        return -1;
    }
    multi_audio_dump_t *_hdl = &audio_dump[dump_src];
    _hdl->handle = ch_map;
    return 0;
}
#endif

void aud_dump_init(void)
{
#ifdef RTOS
    aud_dump_mutex_id = osMutexCreate(osMutex(aud_dump_mutex));
#endif

#ifdef __ARM_ARCH_ISA_ARM
    if (p_transq_msg == NULL) {
        p_transq_msg = a7_dsp_heap_alloc(sizeof(TRANSQ_MSG_T));
    }
    if (p_aud_dump_data == NULL) {
        p_aud_dump_data = a7_dsp_heap_alloc(AUD_DUMP_DATA_LEN);
    }
    printf("%s p_transq_msg=0x%X, p_aud_dump_data=0x%X\n", __FUNCTION__, p_transq_msg, p_aud_dump_data);
#ifdef RTOS
    if (transq_tx_sem == NULL) {
        transq_tx_sem = osSemaphoreCreate(osSemaphore(aud_dump_transq_tx_sem), 0);
        ASSERT(transq_tx_sem!=NULL, "aud_dump_init osSemaphoreCreate fail!");
    }
#endif
    transq_msg_register(TRANSQ_MSG_TYPE_AUDIO_DUMP, set_transq_tx_done, true);
#endif
}

int aud_dump_set(AUD_DUMP_SRC_T dump_src, int onoff)
{
    //TRACE(0, "%s dump_src=%d, onoff=%d", __FUNCTION__, dump_src, onoff);
    if (dump_src >= AUD_DUMP_SRC_NUM) {
        printf("%s %d dump_src=%d overflow [max %d]\n",__func__,__LINE__,dump_src,AUD_DUMP_SRC_NUM);
        return -1;
    }

    // start data dump
    multi_audio_dump_t *_hdl = &audio_dump[dump_src];
    if (onoff) {
#ifdef RTOS
        osMutexWait(aud_dump_mutex_id, osWaitForever);
        _hdl->handle = AUD_DUMP_INIT;
        osMutexRelease(aud_dump_mutex_id);
#else
        _hdl->handle = AUD_DUMP_INIT;
#endif
#ifdef __ARM_ARCH_ISA_ARM
        _hdl->param = data_dump_open(aud_dump_cb_register, 128, MIC_BITRATE, 1);
        data_dump_set_timeout(_hdl->param, 3000);
#else
        transq_msg_register(TRANSQ_MSG_TYPE_AUDIO_DUMP, aud_dump_handler, false);
#endif
    } else {
#ifdef RTOS
        osMutexWait(aud_dump_mutex_id, osWaitForever);
        _hdl->handle = AUD_DUMP_IDLE;
        osMutexRelease(aud_dump_mutex_id);
#else
        _hdl->handle = AUD_DUMP_IDLE;
#endif
#ifdef __ARM_ARCH_ISA_ARM
        data_dump_close(_hdl->param, aud_dump_cb_unregister);
#else
        transq_msg_register(TRANSQ_MSG_TYPE_AUDIO_DUMP, NULL, false);
#endif
#ifdef RTOS
        osMutexWait(aud_dump_mutex_id, osWaitForever);
        memset(_hdl, 0, sizeof(multi_audio_dump_t));
        osMutexRelease(aud_dump_mutex_id);
#else
        memset(_hdl, 0, sizeof(multi_audio_dump_t));
#endif
    }
#ifndef __ARM_ARCH_ISA_ARM
    A7_CMD_T cmd;
    cmd.type = A7_CMD_TYPE_AUDIO_DUMP;
    cmd.p1 = dump_src;
    cmd.p2 = onoff;
    mcu_cmd_send(&cmd);
#endif

    printf("%s dump_src=%d, onoff=%d\n", __FUNCTION__, dump_src, onoff);
    return 0;
}
