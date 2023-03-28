#ifndef NOSTD
#include <stdbool.h>
#include "string.h"
#include "stdio.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_location.h"
#include "transq_msg.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_bootmode.h"
#include "mcu_audio.h"
#include "heart_beat.h"

#ifdef __ARM_ARCH_ISA_ARM
#ifdef A7_ALLOC_TRANSQ_BUF
static SYNC_FLAGS_LOC TRANSQ_MSG_T transq_msg;//must be uncached static parameter
static TRANSQ_MSG_T *p_transq_msg = &transq_msg;
#else
static TRANSQ_MSG_T *p_transq_msg = NULL;
#endif

#ifndef RTOS
static int transq_tx_done = 0;
static void set_transq_tx_done(void *param)
{
    transq_tx_done = 1;
}
#else
osSemaphoreDef(heartbeat_transq_tx_sem);
static osSemaphoreId transq_tx_sem = NULL;
static void set_transq_tx_done(void *param)
{
    osSemaphoreRelease(transq_tx_sem);
}
#endif

static int my_heartbeat_thread(void)
{
    int idx = 0;
    printf("\n\n%-32s start......\n", __FUNCTION__);
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_transq_msg->type = TRANSQ_MSG_TYPE_HEARTBEAT;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;

    while (1) {
        p_transq_msg->id = idx++;
        transq_msg_tx_wait_done(p_transq_msg);
        //printf("SEND heart beat %d\n", idx);

        osDelay(1000);
    }

    printf("%s exit......", __FUNCTION__);
    return 0;
}
osThreadId my_heartbeat_thread_id;
osThreadDef(my_heartbeat_thread, osPriorityAboveNormal, 1, 4*1024, "my_heartbeat_thread");
#else
static osTimerId _a7_heartbeat_timer_id;
#ifdef KERNEL_LITEOS_M
extern uint32_t OsGetAllTskInfo(void);
#endif
static uint8_t a7_timeout_cnt = 0;
//this function is defined at genie aos.c
void WEAK a7_dsp_reboot()
{
    printf("warning, a7_dsp_reboot is a WEAK function\n");
}

static int a7_dsp_crash;

static int heartbeat_reboot_enable = 1;
static int heartbeat_timer_error = 0;
void a7_heartbeat_reboot_enable(int enable)
{
    heartbeat_reboot_enable = enable;
}

void a7_dsp_set_crash_status(int crash)
{
    a7_dsp_crash = crash;
}

static void _a7_heartbeat_timeout(void const *arg)
{
    printf("a7 heartbeat timeout(enable:%d)\n", heartbeat_reboot_enable);
 #ifdef KERNEL_LITEOS_M
    OsGetAllTskInfo();
 #endif
    if (heartbeat_timer_error) {
        printf("restart heartbeat timer fail, doesn't reset a7\n");
        return;
    }
    a7_timeout_cnt++;
    if (a7_timeout_cnt > A7_DSP_BOOT_RETRY_NUM) {
        TRACE(0|TR_ATTR_IMM, "timeout cnt:%d, now reboot m33, set a7 bootmode", a7_timeout_cnt);
        hal_sw_bootmode_set(HAL_SW_BOOTMODE_A7_REBOOT);
        hal_cmu_sys_reboot();
        return;
    }
    a7_dsp_crash = 1;
    if (heartbeat_reboot_enable) {
        a7_dsp_recover_report_event(A7_RECOVERING);
        a7_dsp_reboot();
    }
}

//this function is defined at genie aos.c
void WEAK a7_dsp_recover_report()
{
}
osTimerDef(_a7_heartbeat_timer, _a7_heartbeat_timeout);

void heartbeat_stop_timer(void)
{
    if (_a7_heartbeat_timer_id) {
        printf("a7 heartbeat timer stop\n");
        int ret = osTimerStop(_a7_heartbeat_timer_id);
        if (ret != osOK) {
            printf("a7 heartbeat timer stop failed! ret:%d\n", ret);
        }
    }
}

void heartbeat_reset_timer(int tick_ms)
{
    if (_a7_heartbeat_timer_id) {
        osTimerStop(_a7_heartbeat_timer_id);
        osStatus ret = osTimerStart(_a7_heartbeat_timer_id, tick_ms);
        if (ret == osErrorOS) {
            heartbeat_timer_error = 1;
        } else {
            heartbeat_timer_error = 0;
        }
        //printf("%s time=%dms heartbeat_timer_error=%d\n", __FUNCTION__, tick_ms, heartbeat_timer_error);
    }
}

extern uint32_t dsp_start_ticks;
#define A7_RECOVER_TIME     1500
static void heartbeat_callback(void *param)
{
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    if (msg->type != TRANSQ_MSG_TYPE_HEARTBEAT) {
        printf("%s type=%d, should be TRANSQ_MSG_TYPE_HEARTBEAT!\n", __FUNCTION__, msg->type);
        return;
    }
    /* reset timeout count */
    a7_timeout_cnt = 0;
    A7_RECOVER_EVENT status = a7_dsp_status_get();
    if (status == A7_USER_DISABLE) {
        printf("%s dsp status:%d, wait status stable!\n", __FUNCTION__, status);
        return;
    }
    static int idx = -1;
    if (idx == -1) {
        idx = msg->id;
        transq_msg_onoff(1);
    }
    if (msg->id != idx++) {
        printf("%s:%d heart beat recieve MISSING msg, cur idx=%d[last %d, lost %d]\n", __FUNCTION__, __LINE__, msg->id, (idx-1), (msg->id+1-idx));
        osDelay(100);
        //ASSERT(0,"heart beat lost!");
        idx = -1;//a7 may by restarted
    }
    if (a7_dsp_crash) {
        printf("a7_has recoverd\n");
        a7_dsp_recover_report();
        uint32_t endtick = hal_sys_timer_get();
        uint32_t costtime = TICKS_TO_MS(endtick - dsp_start_ticks);
        printf("%s %d start:%u, end:%u costtime:%ums\n", __FUNCTION__, __LINE__, dsp_start_ticks, endtick, costtime);
        a7_dsp_recover_report_event(A7_RECOVER_DONE);
        a7_dsp_crash = 0;
        if (costtime > A7_RECOVER_TIME) {
#ifdef KERNEL_LITEOS_M
           OsGetAllTskInfo();
#endif
        }
    }
    //printf("GOT heart beat %d\n", idx);
    heartbeat_reset_timer(5*1000);
}

#ifdef RTOS
static osMutexId a7_dsp_status_mutex_id = NULL;
osMutexDef(a7_status_mutex);
#endif
static void a7_dsp_status_mutex_init(void)
{
    a7_dsp_status_mutex_id = osMutexCreate(osMutex(a7_status_mutex));
    if (a7_dsp_status_mutex_id == NULL) {
        printf("%s Create mutex failed!!\n", __FUNCTION__);
    }
}

void a7_dsp_status_mutex_wait(void)
{
    if (a7_dsp_status_mutex_id) {
        osMutexWait(a7_dsp_status_mutex_id, osWaitForever);
    } else {
        a7_dsp_status_mutex_init();
    }
}

void a7_dsp_status_mutex_release(void)
{
    if (a7_dsp_status_mutex_id) {
        osMutexRelease(a7_dsp_status_mutex_id);
    }
}

static A7_RECOVER_EVENT a7_dsp_status = A7_RECOVER_DONE;
static void a7_dsp_status_set(A7_RECOVER_EVENT status)
{
    a7_dsp_status_mutex_wait();
    a7_dsp_status = status;
    printf("%s a7_dsp_status:%d\n", __FUNCTION__, a7_dsp_status);
    a7_dsp_status_mutex_release();
}
A7_RECOVER_EVENT a7_dsp_status_get(void)
{
    A7_RECOVER_EVENT status;
    a7_dsp_status_mutex_wait();
    status = a7_dsp_status;
    a7_dsp_status_mutex_release();
    return status;
}


static a7_recover_cb_t _a7_recover_cbf[A7_RESET_RECOVER_CB_MAX] = {NULL};

a7_recover_cb_t *a7_dsp_get_recover_cb(void)
{
    return &_a7_recover_cbf[0];
}

void a7_dsp_recover_report_event(A7_RECOVER_EVENT event)
{
    a7_dsp_status_set(event);
    a7_recover_cb_t *a7_cb = a7_dsp_get_recover_cb();
    for (int i = 0; i < A7_RESET_RECOVER_CB_MAX; i++) {
        if (a7_cb[i] != NULL) {
            a7_cb[i](event);
        }
    }
}

void a7_dsp_recover_subscribe(a7_recover_cb_t cbf)
{
    int i = 0;
    for (i = 0; i < A7_RESET_RECOVER_CB_MAX; i++) {
        if ((!_a7_recover_cbf[i]) && (cbf)) {
            _a7_recover_cbf[i] = cbf;

            printf("%s cb[%d]:%p register\n", __FUNCTION__, i, cbf);
            break;
        }
    }
    if (i == A7_RESET_RECOVER_CB_MAX) {
        printf("%s cb number is beyond max:%d!!\n", __FUNCTION__, A7_RESET_RECOVER_CB_MAX);
    }
}

void a7_dsp_recover_unsubscribe(a7_recover_cb_t cbf)
{
    for (int i = 0; i < A7_RESET_RECOVER_CB_MAX ; i++) {
        if ((cbf) && (_a7_recover_cbf[i] == cbf)) {
            printf("%s cb[%d]:%p\r\n", __FUNCTION__, i, cbf);
            _a7_recover_cbf[i] = NULL;
            break;
        }
    }
}

#endif

void heartbeat_init(void)
{
#ifdef __ARM_ARCH_ISA_ARM
    printf("%s a7 \n",__func__);
    if (p_transq_msg == NULL) {
        p_transq_msg = a7_dsp_heap_alloc(sizeof(TRANSQ_MSG_T));
    }
    printf("%s p_transq_msg=0x%X\n", __FUNCTION__, p_transq_msg);
    if (transq_tx_sem == NULL) {
        transq_tx_sem = osSemaphoreCreate(osSemaphore(heartbeat_transq_tx_sem), 0);
        ASSERT(transq_tx_sem!=NULL, "heartbeat_init osSemaphoreCreate fail!");
    }
    transq_msg_register(TRANSQ_MSG_TYPE_HEARTBEAT, set_transq_tx_done, true);
    my_heartbeat_thread_id = osThreadCreate(osThread(my_heartbeat_thread), NULL);
    if (my_heartbeat_thread_id == NULL){
        printf("%s %d fail to create my_heartbeat_thread task\n",__func__,__LINE__);
        osSemaphoreDelete(transq_tx_sem);
        transq_tx_sem = NULL;
    }
    printf("%s create my_heartbeat_thread_id task = 0x%X", __func__, (uint32_t)my_heartbeat_thread_id);
#else
    printf("%s mcu \n",__func__);
    transq_msg_register(TRANSQ_MSG_TYPE_HEARTBEAT, heartbeat_callback, false);

    if (_a7_heartbeat_timer_id == NULL) {
        _a7_heartbeat_timer_id = osTimerCreate(osTimer(_a7_heartbeat_timer), osTimerOnce, NULL);
        if (_a7_heartbeat_timer_id == NULL) {
            printf("creat a7 heartbeat timer fial\n");
        }
    }
#endif
}

#else // NOSTD
void heartbeat_init(void)
{
     printf("%s weak \n",__func__);
}
#endif
