#ifdef SPEECH_MSG_ENABLE
#include "speech_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "string.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_trace.h"
#include "hal_location.h"
#include "transq_msg.h"
#include "mcu_audio.h"

static int _sp_init = 0;

#define MSG_DATA_MAX_LEN               (1024*42)

/* Use differ send timeout because of m33 rx cb will missing data(sometime) when press test (maybe some task hold time tool long), 
 * Audio player need callback as soon as possible when a7 die
 */
#ifdef __ARM_ARCH_ISA_ARM
#define SPEECH_MSG_SEND_TIMEOUT        3000
#else
#define SPEECH_MSG_SEND_TIMEOUT        1500
#endif

#ifdef A7_ALLOC_TRANSQ_BUF
static SYNC_FLAGS_LOC TRANSQ_MSG_T transq_msg;	                        //must be uncached static parameter
static SYNC_FLAGS_LOC unsigned char speech_msg_data[MSG_DATA_MAX_LEN];	//must be uncached static parameter
static TRANSQ_MSG_T *p_transq_msg = &transq_msg;
static unsigned char *p_speech_msg_data = speech_msg_data;
#else
static TRANSQ_MSG_T *p_transq_msg = NULL;
static unsigned char *p_speech_msg_data = NULL;
#endif

#ifdef RTOS
osMutexDef(speech_msg_mutex);
static osMutexId speech_msg_mutex_id = 0;
#endif

#define MSG_BC_MAX  4
static speech_msg_callback_t user_data_cbf[MSG_BC_MAX] = {NULL, NULL};
static speech_msg_callback_t codec_cbf[MSG_BC_MAX] = {NULL, NULL};
static void speech_msg_capture_cb(void *param)
{
    if (param == NULL) {
        TRACE(2, "%s param is null!", __FUNCTION__);
        return;
    }
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    unsigned int len = msg->user_data_len;
    if (msg->type == TRANSQ_MSG_TYPE_USERDATA) {
        if (user_data_cbf[0] || user_data_cbf[1]) {
            static int user_data_idx = -1;
            if (user_data_idx == -1) {
                user_data_idx = msg->id;
            }
            if (msg->id != user_data_idx++) {
                printf("%s:%d recieve MISSING msg, cur idx=%d[last %d, lost %d] data=0x%p\n", __FUNCTION__, __LINE__,
                    msg->id, (user_data_idx-1), (msg->id+1-user_data_idx), msg->user_data);
                osDelay(100);
                user_data_idx = -1;//a7 may by restarted
            }
            if ((user_data_cbf[0]) && (msg->user_data != NULL)) {
                user_data_cbf[0](msg->user_data, len);
            }
            if ((user_data_cbf[1]) && (msg->user_data != NULL)) {
                user_data_cbf[1](msg->user_data, len);
            }
        }
    } else if (msg->type == TRANSQ_MSG_TYPE_CODEC) {
        if ((codec_cbf[0]) && (msg->user_data != NULL)) {
            codec_cbf[0](msg->user_data, len);
        }
        if ((codec_cbf[1]) && (msg->user_data != NULL)) {
            codec_cbf[1](msg->user_data, len);
        }
    } else
        TRACE(2, "%s invalid type %d", __FUNCTION__, msg->type);
        return;
}

#ifndef RTOS
static int transq_tx_done = 0;
static void set_transq_tx_done(void *param)
{
    transq_tx_done = 1;
}
#else
osSemaphoreDef(speech_transq_tx_sem);
static osSemaphoreId transq_tx_sem = NULL;
static void set_transq_tx_done(void *param)
{
    if(transq_tx_sem)
        osSemaphoreRelease(transq_tx_sem);
}
#endif

static void __speech_init(void)
{
    printf("%s\n",__func__);
#ifdef __ARM_ARCH_ISA_ARM
    if (p_transq_msg == NULL) {
        p_transq_msg = a7_dsp_heap_alloc(sizeof(TRANSQ_MSG_T));
    }
    if (p_speech_msg_data == NULL) {
        p_speech_msg_data = a7_dsp_heap_alloc(MSG_DATA_MAX_LEN);
    }
#else
    extern void *nts_malloc(size_t size);
    if (p_transq_msg == NULL) {
        p_transq_msg = nts_malloc(sizeof(TRANSQ_MSG_T));
    }
    if (p_speech_msg_data == NULL) {
        p_speech_msg_data = nts_malloc(MSG_DATA_MAX_LEN);
    }
#endif
    if (speech_msg_mutex_id == NULL){
        speech_msg_mutex_id =osMutexCreate(osMutex(speech_msg_mutex));
        ASSERT(speech_msg_mutex_id!=NULL, "__speech_init osMutexCreate fail!");
    }
    if (transq_tx_sem == NULL) {
        transq_tx_sem = osSemaphoreCreate(osSemaphore(speech_transq_tx_sem), 0);
        ASSERT(transq_tx_sem!=NULL, "__speech_init osSemaphoreCreate fail!");
    }

    printf("%s p_transq_msg=0x%p, p_speech_msg_data=0x%p\n", __FUNCTION__, p_transq_msg, p_speech_msg_data);

    transq_msg_register(TRANSQ_MSG_TYPE_USERDATA, speech_msg_capture_cb, false);
    transq_msg_register(TRANSQ_MSG_TYPE_USERDATA, set_transq_tx_done, true);
#ifdef __A7_DSP_CODEC__
    transq_msg_register(TRANSQ_MSG_TYPE_CODEC, speech_msg_capture_cb, false);
    transq_msg_register(TRANSQ_MSG_TYPE_CODEC, set_transq_tx_done, true);
#endif

    _sp_init = 1;
}
/**************** 
 * return value:
 * == 0 is sucess
 * != 0 is tiemout/error
 ***************/
static int speech_msg_transq_with_timeout(TRANSQ_MSG_T *msg, uint32_t timeout)
{
    int32_t count = -1;
    int ret;
    if (msg == NULL) {
        TRACE(1, "%s msg is NULL!", __func__);
        return -1;
    }
    ret = transq_msg_tx(msg);
    if (ret == 1) {  //HAL_TRANSQ_RET_OK
        if (transq_tx_sem != NULL) {
            count = osSemaphoreWait(transq_tx_sem, timeout);
            if(count > 0) {
                return 0;
            }
            TRACE(0, "%s wait sem fail timeout:%d count:%d!", __func__, timeout, count);
        }
    }
    return -1;
}

/* when success ,return size */
int speech_msg_send(void* msg, int size)
{
    int ret = -1;

    if ((!msg) || (size < 1)) {
        TRACE(1, "%s msg is NULL!or size:%d\r\n",__func__,size);
        return 0;
    }
    if (!_sp_init) {
        __speech_init();
    }
    if (size >= MSG_DATA_MAX_LEN) {
        TRACE(1, "%s size=%d, should < MSG_DATA_MAX_LEN:%d!!!", __FUNCTION__, size, MSG_DATA_MAX_LEN);
        return 0;
    }
#ifdef RTOS
    osMutexWait(speech_msg_mutex_id, osWaitForever);
#endif
    static unsigned int idx = 0;
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_transq_msg->type = TRANSQ_MSG_TYPE_USERDATA;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
    memcpy(p_speech_msg_data, (void *)msg, size);

    p_speech_msg_data[size] = 0;
    p_transq_msg->id = idx++;
    p_transq_msg->user_data = p_speech_msg_data;
    p_transq_msg->user_data_len = size;
    p_transq_msg->sync = 0;
    //transq_msg_tx_wait_done(p_transq_msg);
    ret = speech_msg_transq_with_timeout(p_transq_msg, SPEECH_MSG_SEND_TIMEOUT);
#ifdef RTOS
    osMutexRelease(speech_msg_mutex_id);
#endif
    //printf("%s p_transq_msg->id:%d.\r\n",__FUNCTION__,p_transq_msg->id);
    if (ret != 0) {
        return 0;
    }
    return size;
}

/* Don't block in cb func */
void speech_msg_subscribe(speech_msg_callback_t cbf)
{
    int j = 0;
    printf("%s cbf:%p \r\n", __FUNCTION__, cbf);

    if (!_sp_init) {
        __speech_init();
    }

    for (j = 0; j < MSG_BC_MAX ; j++){
        if ((!user_data_cbf[j]) && (cbf)) {
            user_data_cbf[j] = cbf;
            printf("%s cb[%d]:%p register\n", __FUNCTION__, j, cbf);
            break;
        }
    }
    if (j == MSG_BC_MAX) {
        printf("%s cb number is beyond max:%d!!\n", __FUNCTION__, MSG_BC_MAX);
    }
}

void speech_msg_unsubscribe(speech_msg_callback_t cbf)
{
    for (int j = 0; j < MSG_BC_MAX ; j++) {
        if ((cbf) && (user_data_cbf[j] == cbf)) {
            printf("%s cb[%d]:%p\r\n", __FUNCTION__, j, cbf);
            user_data_cbf[j] = NULL;
            break;
        }
    }
}

int speech_msg_send_ext(TRANSQ_MSG_TYPE_T type, uint8_t cmd, void* msg, int size)
{
    int ret = -1;

    if (!_sp_init) {
        __speech_init();
    }
    if (size >= MSG_DATA_MAX_LEN-1) {
        TRACE(1, "%s size=%d, should < MSG_DATA_MAX_LEN:%d!!!", __FUNCTION__, size, MSG_DATA_MAX_LEN);
        return 0;
    }
#ifdef RTOS
    osMutexWait(speech_msg_mutex_id, osWaitForever);
#endif
    static unsigned int idx = 0;
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_transq_msg->type = type;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
    p_speech_msg_data[0] = cmd;
    if (msg && size > 0) {
        memcpy(&p_speech_msg_data[1], (void *)msg, size);
    }
    size++; // include cmd byte

    p_speech_msg_data[size] = 0;
    p_transq_msg->id = idx++;
    p_transq_msg->user_data = p_speech_msg_data;
    p_transq_msg->user_data_len = size;
    p_transq_msg->sync = 0;
    //transq_msg_tx_wait_done(p_transq_msg);
    ret = speech_msg_transq_with_timeout(p_transq_msg, SPEECH_MSG_SEND_TIMEOUT);
#ifdef RTOS
    osMutexRelease(speech_msg_mutex_id);
#endif
    //printf("%s p_transq_msg->id:%d type %d\r\n",__FUNCTION__,p_transq_msg->id, p_transq_msg->type);
    if (ret != 0) {
        return 0;
    }
    return size;
}

/* Don't block in cb func */
void speech_msg_subscribe_ext(TRANSQ_MSG_TYPE_T type, speech_msg_callback_t cbf)
{
    int j;
    speech_msg_callback_t * cbf_tbl;
    printf("%s type %d cbf:%p \r\n", __FUNCTION__, type, cbf);

    if (!_sp_init) {
        __speech_init();
    }

    switch (type) {
        case TRANSQ_MSG_TYPE_USERDATA:
            cbf_tbl = user_data_cbf;
            break;
        case TRANSQ_MSG_TYPE_CODEC:
            cbf_tbl = codec_cbf;
            break;
        default:
            return;
    }

    for (j = 0; j < MSG_BC_MAX ; j++){
        if ((!cbf_tbl[j]) && (cbf)) {
            cbf_tbl[j] = cbf;
            printf("%s cb[%d]:%p register\n", __FUNCTION__, j, cbf);
            break;
        }
    }

    if (j == MSG_BC_MAX) {
        printf("%s cb number is beyond max:%d!!\n", __FUNCTION__, MSG_BC_MAX);
    }
}

void speech_msg_unsubscribe_ext(TRANSQ_MSG_TYPE_T type, speech_msg_callback_t cbf)
{
    speech_msg_callback_t * cbf_tbl;

    switch (type) {
        case TRANSQ_MSG_TYPE_USERDATA:
            cbf_tbl = user_data_cbf;
            break;
        case TRANSQ_MSG_TYPE_CODEC:
            cbf_tbl = codec_cbf;
            break;
        default:
            return;
    }

    for (int j = 0; j < MSG_BC_MAX ; j++) {
        if ((cbf) && (cbf_tbl[j] == cbf)) {
            printf("%s cb[%d]:%p\r\n", __FUNCTION__, j, cbf);
            cbf_tbl[j] = NULL;
            break;
        }
    }
}
#endif
