#include <stdbool.h>
#include "string.h"
#include "hal_location.h"
#include "transq_msg.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "a7_cmd.h"
#include "mcu_audio.h"
#ifdef RTOS
#include "bes_kv/bes_kv.h"
#endif
#ifndef NUTTX_BUILD
#include "sys_time.h"
#endif

#ifdef A7_ALLOC_TRANSQ_BUF
static SYNC_FLAGS_LOC TRANSQ_MSG_T transq_msg;//must be uncached static parameter
static TRANSQ_MSG_T *p_transq_msg = &transq_msg;
static SYNC_FLAGS_LOC A7_CMD_T a7_cmd;//must be uncached static parameter
static A7_CMD_T *p_a7_cmd = &a7_cmd;
static SYNC_FLAGS_LOC char a7_cmd_data[A7_CMD_MAX_LEN];//must be uncached static parameter
static char *p_a7_cmd_data = a7_cmd_data;
#else
static TRANSQ_MSG_T *p_transq_msg = NULL;
static A7_CMD_T *p_a7_cmd = NULL;
static char *p_a7_cmd_data = NULL;
#endif

extern void a7_debug_mm_interval_set(int interval);
extern void a7_uart_switch(enum HAL_TRACE_TRANSPORT_T transport);
extern void a7_dsp_heap_init(unsigned char *buf, int len);
extern osStatus_t osThreadBindCpu(osThreadId_t thread_id, uint8_t cpu_id);

void WEAK a7_debug_mm_interval_set(int interval)
{
    TRACE(0, "%s weak", __FUNCTION__);
}

void WEAK a7_debug_mm_print(void)
{
    TRACE(0, "%s weak", __FUNCTION__);
}

void WEAK a7_ctrl_plane_handler(unsigned int cmd1,unsigned int cmd2)
{
    TRACE(0, "%s weak cmd1:%d cmd2:%d", __FUNCTION__, cmd1,cmd2);
}

static void a7_panic_task(void const*argument)
{
    osDelay(100);
    __asm__ __volatile__("udf":::"memory");
}
osThreadDef(a7_panic_task, osPriorityNormal, 1, 4 * 1024, "a7_panic_task");

static void a7_cmd_callback(void *param)
{
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    A7_CMD_T *cmd = msg->user_data;
    osThreadId_t a7_panic_thread_id = NULL;
    if (msg->type != TRANSQ_MSG_TYPE_CMD) {
        TRACE(0, "%s type=%d, should be TRANSQ_MSG_TYPE_CMD!", __FUNCTION__, msg->type);
        return;
    }

    switch (cmd->type) {
        case A7_CMD_TYPE_DEBUG_MM:
            if (cmd->p1 == 0) {
                a7_debug_mm_interval_set(0);
                a7_debug_mm_print();
            } else {
                a7_debug_mm_interval_set(cmd->p1);
            }
            break;
        case A7_CMD_TYPE_AUDIO_DUMP:
            aud_dump_set((AUD_DUMP_SRC_T)cmd->p1, cmd->p2);
            break;
        case A7_CMD_TYPE_UART2_TRACE:
            if (cmd->p1 == 0) {
                a7_uart_switch(HAL_TRACE_TRANSPORT_TRANSQ1);
            } else {
                a7_uart_switch(HAL_TRACE_TRANSPORT_UART2);
            }
            break;
        case A7_CMD_TYPE_STATPRINT:
#ifdef RTOS
            bes_kv_item_get(cmd->p1>0, cmd->p1);
#endif
            break;
        case A7_CMD_TYPE_PANIC:
            a7_panic_thread_id = (osThreadId_t)osThreadCreate(osThread(a7_panic_task), (void *)&cmd->p1);
            if (a7_panic_thread_id) {
                if (osThreadBindCpu(a7_panic_thread_id, cmd->p1) != osOK)
                    TRACE(0, "a7_panic_task failed to bind cpu %d", cmd->p1);
            } else
                TRACE(0, "failed to create a7_panic_task");
            break;
        case A7_CMD_TYPE_HEAP_INIT:
            a7_dsp_heap_init((unsigned char *)cmd->p1, cmd->p2);
            if (p_transq_msg == NULL) {
                p_transq_msg = (TRANSQ_MSG_T *)a7_dsp_heap_alloc(sizeof(TRANSQ_MSG_T));
            }
            if (p_a7_cmd == NULL) {
                p_a7_cmd = (A7_CMD_T *)a7_dsp_heap_alloc(sizeof(A7_CMD_T));
            }
            if (p_a7_cmd_data == NULL) {
                p_a7_cmd_data = (char *)a7_dsp_heap_alloc(A7_CMD_MAX_LEN);
            }
            break;
        case A7_CMD_TYPE_HANDSHAKE:
            mic_set_samplerate(cmd->p1);
            mic_set_period(cmd->p2);
            TRACE(0, "%s A7_CMD_TYPE_HANDSHAKE mic_samplerate=%d, period_ms=%d", __FUNCTION__, cmd->p1, cmd->p2);
            break;
        case A7_CMD_TYPE_CUST_CTRL_PLANE:
            a7_ctrl_plane_handler(cmd->p1,cmd->p2);
            break;
        case A7_CMD_TYPE_BOOTNUM:
            printf("%s A7_CMD_TYPE_BOOTNUM %d\n", __FUNCTION__, cmd->p1);
            extern void a7_set_bootnum(int num);
            a7_set_bootnum(cmd->p1);
            break;
        case A7_CMD_TYPE_TIME:
            printf("%s A7_CMD_TYPE_TIME %d:%d\n", __FUNCTION__, cmd->p1, cmd->p2);
            hal_sys_time_init(cmd->p1, cmd->p2);
            break;
#ifdef KERNEL_RHINO
        case A7_CMD_TYPE_TASKBT:
            printf("%s A7_CMD_TYPE_TASKBT %s\n", __FUNCTION__, cmd->str);
            extern void debug_backtrace_task(char *taskname);
            debug_backtrace_task(cmd->str);
            break;
        case A7_CMD_TYPE_MMLK:
            printf("%s A7_CMD_TYPE_MMLK\n", __FUNCTION__);
            extern void a7_debug_mmlk(void);
            a7_debug_mmlk();
            break;
#ifndef A7_ALLOC_TRANSQ_BUF
#ifdef OPENCPU_ALGO_MODEL_SEPERATE
        case A7_CMD_TYPE_ALG_LEN:
        {
            algo_total_len = cmd->p1;
            if (algo_total_len > 0) {
                algo_model = (char *)malloc(algo_total_len);
            }
        } break;
        case A7_CMD_TYPE_WAKEUP_LEN:
        {
            wakeup_total_len = cmd->p1;
            if (wakeup_total_len > 0) {
                wakeup_conf = (char *)malloc(wakeup_total_len);
            }
        } break;
        case A7_CMD_TYPE_ALG_DATA:
        {
            algo_addr = (uint8_t *)cmd->p1;
            if (algo_addr) {
                memcpy(&algo_model[algo_len], algo_addr, cmd->p2);
                algo_len += cmd->p2;
            }
        } break;
        case A7_CMD_TYPE_WAKEUP_CONFIG:
        {
            wakeup_addr = (uint8_t *)cmd->p1;
            if (wakeup_addr) {
                memcpy(&wakeup_conf[wakeup_len], wakeup_addr, cmd->p2);
                wakeup_len += cmd->p2;
            }
        } break;
        case A7_CMD_TYPE_ALGO_MODEL_INIT:
        {
            loadWakeupEngine(NULL, 0, algo_model, algo_len);
            wakeupStart(wakeup_conf, wakeup_env);
        } break;
#endif
#endif
#endif // KERNEL_RHINO
        default:
            break;
    }
}

#ifndef RTOS
static int transq_tx_done = 0;
static void set_transq_tx_done(void *param)
{
    transq_tx_done = 1;
}
#else
static osSemaphoreId transq_tx_sem = NULL;
static void set_transq_tx_done(void *param)
{
    osSemaphoreRelease(transq_tx_sem);
}
#endif

void a7_cmd_init(void)
{
    transq_msg_register(TRANSQ_MSG_TYPE_CMD, a7_cmd_callback, false);
    transq_msg_register(TRANSQ_MSG_TYPE_CMD, set_transq_tx_done, true);
}

void a7_cmd_send2(A7_CMD_T *cmd)
{
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    memcpy(p_a7_cmd, cmd, sizeof(A7_CMD_T));
    p_transq_msg->type = TRANSQ_MSG_TYPE_CMD;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
    p_transq_msg->user_data = (void *)p_a7_cmd;
    p_transq_msg->user_data_len = sizeof(A7_CMD_T);
    transq_msg_tx(p_transq_msg);

    TRACE(0, "%s end", __FUNCTION__);
}

void a7_cmd_send(void *data, int len)
{
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    memset(p_a7_cmd_data, 0, A7_CMD_MAX_LEN);
    memcpy(p_a7_cmd_data, data, len);
    p_transq_msg->type = TRANSQ_MSG_TYPE_CMD;
    p_transq_msg->pri = HAL_TRANSQ_PRI_HIGH;
    p_transq_msg->user_data = (void *)p_a7_cmd_data;
    p_transq_msg->user_data_len = len;
    transq_msg_tx(p_transq_msg);

    TRACE(0, "%s end", __FUNCTION__);
}

