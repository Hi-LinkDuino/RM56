#include <stdbool.h>
#include "string.h"
#include "cmsis_os.h"
#include "hal_location.h"
#include "transq_msg.h"
#include "a7_cmd.h"
#include "hal_trace.h"
#include "hal_timer.h"

static int init = 0;
static SYNC_FLAGS_LOC TRANSQ_MSG_T transq_msg;//must be uncached static parameter
static SYNC_FLAGS_LOC A7_CMD_T a7_cmd;//must be uncached static parameter

osSemaphoreDef(mcu_cmd_transq_tx_sem);
static osSemaphoreId transq_tx_sem = NULL;
static void set_transq_tx_done(void *param)
{
    osSemaphoreRelease(transq_tx_sem);
}

static a7_cmd_callback_t cb = NULL;
extern void hal_trace_print_a7_flush(int onoff);
extern void heartbeat_reset_timer(int tick_ms);
static void mcu_cmd_callback(void *param)
{
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    A7_CMD_T *cmd = msg->user_data;
    if (msg->type != TRANSQ_MSG_TYPE_CMD) {
        printf("%s type=%d, should be TRANSQ_MSG_TYPE_CMD!\n", __FUNCTION__, msg->type);
        return;
    }

    switch (cmd->type) {
        case A7_CMD_TYPE_PANIC:
            hal_trace_print_a7_flush(1);
            heartbeat_reset_timer(2*1000);
            break;
        default:
            break;
    }

    if (cb != NULL)
    {
        cb((char *)msg->user_data, msg->user_data_len);
    }
}

static void _mcu_cmd_init(void)
{
    if (transq_tx_sem == NULL) {
        transq_tx_sem = osSemaphoreCreate(osSemaphore(mcu_cmd_transq_tx_sem), 0);
        TRACE(0, "mcu_cmd_transq_tx_sem = %p", transq_tx_sem);
        ASSERT(transq_tx_sem!=NULL, "_mcu_cmd_init osSemaphoreCreate fail!");
    }
    transq_msg_register(TRANSQ_MSG_TYPE_CMD, mcu_cmd_callback, false);
    transq_msg_register(TRANSQ_MSG_TYPE_CMD, set_transq_tx_done, true);
    init = 1;
}

void mcu_cmd_send(A7_CMD_T *cmd)
{
    if (!init) {
        _mcu_cmd_init();
    }
    memset(&transq_msg, 0, sizeof(transq_msg));
    memcpy(&a7_cmd, cmd, sizeof(A7_CMD_T));
    transq_msg.type = TRANSQ_MSG_TYPE_CMD;
    transq_msg.pri = HAL_TRANSQ_PRI_NORMAL;
    transq_msg.user_data = (void *)&a7_cmd;
    transq_msg.user_data_len = sizeof(A7_CMD_T);
    transq_msg_tx(&transq_msg);

    TRACE(0, "%s end", __FUNCTION__);
}

void mcu_cmd_send_sync(A7_CMD_T *cmd)
{
    if (!init) {
        _mcu_cmd_init();
    }
    memset(&transq_msg, 0, sizeof(transq_msg));
    memcpy(&a7_cmd, cmd, sizeof(A7_CMD_T));
    transq_msg.type = TRANSQ_MSG_TYPE_CMD;
    transq_msg.pri = HAL_TRANSQ_PRI_NORMAL;
    transq_msg.user_data = (void *)&a7_cmd;
    transq_msg.user_data_len = sizeof(A7_CMD_T);
    transq_msg_tx_wait_done(&transq_msg);

    TRACE(0, "%s end\n", __FUNCTION__);
}

extern void hal_trace_print_a7_flush(int onoff);
extern void heartbeat_reset_timer(int tick_ms);
static void a7_cmd_recv_callback(void *param)
{
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    if (msg->type != TRANSQ_MSG_TYPE_CMD) {
        TRACE(0, "%s type=%d, should be TRANSQ_MSG_TYPE_CMD", __FUNCTION__, msg->type);
        return;
    }
    TRACE(0, "%s user_data:%s  user_data_len:%d",__func__,(char *)msg->user_data,msg->user_data_len);
    A7_CMD_T *cmd = msg->user_data;
    switch (cmd->type) {
        case A7_CMD_TYPE_PANIC:
            hal_trace_print_a7_flush(1);
            heartbeat_reset_timer(2*1000);
            break;
        default:
            break;
    }

    if (cb != NULL)
    {
        cb((char *)msg->user_data, msg->user_data_len);
    }
    
}

void a7_cmd_recv_init(void)
{
    if (transq_tx_sem == NULL) {
        transq_tx_sem = osSemaphoreCreate(osSemaphore(mcu_cmd_transq_tx_sem), 0);
        printf("mcu_cmd_transq_tx_sem = %p\n", transq_tx_sem);
        ASSERT(transq_tx_sem!=NULL, "_mcu_cmd_init osSemaphoreCreate fail!");
    }
    transq_msg_register(TRANSQ_MSG_TYPE_CMD, a7_cmd_recv_callback, false);
    transq_msg_register(TRANSQ_MSG_TYPE_CMD, set_transq_tx_done, true);
    init = 1;
}

void set_a7_cmd_callback_handler (a7_cmd_callback_t callback)
{
    cb = callback;
}
