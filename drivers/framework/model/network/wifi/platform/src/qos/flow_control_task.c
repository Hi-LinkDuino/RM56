/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "flow_control_task.h"
#include "osal_time.h"
#include "hdf_log.h"
#include "flow_control.h"
#define HDF_LOG_TAG "WiFiFlowControl"
#define FLOW_CONTROL_MAP_SIZE 3
struct FcPriority {
    FlowDir dir;
    FlowControlQueueID id;
};

static FlowControlQueueID g_staPriorityMapTx[QUEUE_ID_COUNT] = {
    CTRL_QUEUE_ID, VIP_QUEUE_ID, NORMAL_QUEUE_ID, TCP_ACK_QUEUE_ID, TCP_DATA_QUEUE_ID, VO_QUEUE_ID, VI_QUEUE_ID,
    BE_QUEUE_ID, BK_QUEUE_ID
};

static FlowControlQueueID g_staPriorityMapRx[QUEUE_ID_COUNT] = {
    CTRL_QUEUE_ID, VIP_QUEUE_ID, NORMAL_QUEUE_ID, TCP_DATA_QUEUE_ID, TCP_ACK_QUEUE_ID, VO_QUEUE_ID, VI_QUEUE_ID,
    BE_QUEUE_ID, BK_QUEUE_ID
};

static FlowControlQueueID g_priorityMapTx[QUEUE_ID_COUNT] = {
    CTRL_QUEUE_ID, VIP_QUEUE_ID, NORMAL_QUEUE_ID, TCP_DATA_QUEUE_ID, TCP_ACK_QUEUE_ID, VO_QUEUE_ID, VI_QUEUE_ID,
    BE_QUEUE_ID, BK_QUEUE_ID
};

static FlowControlQueueID g_priorityMapRx[QUEUE_ID_COUNT] = {
    CTRL_QUEUE_ID, VIP_QUEUE_ID, NORMAL_QUEUE_ID, TCP_ACK_QUEUE_ID, TCP_DATA_QUEUE_ID, VO_QUEUE_ID, VI_QUEUE_ID,
    BE_QUEUE_ID, BK_QUEUE_ID
};

static bool IsFcThreadNeedStop(struct FlowControlModule *fcm, FlowDir dir)
{
    if (fcm->threadStatus[dir] > THREAD_RUNNING) {
        return true;
    }
    return false;
}

static void FlowControlTxTreadProcess(struct FlowControlModule *fcm)
{
    bool isSta = false;
    int i;
    if (fcm->op != NULL && fcm->op->isDeviceStaOrP2PClient != NULL) {
        isSta = fcm->op->isDeviceStaOrP2PClient();
    }
    if (isSta) {
        for (i = 0; i < FLOW_CONTROL_MAP_SIZE; i++) {
            SendFlowControlQueue(fcm, g_staPriorityMapTx[i], FLOW_TX);
        }
    } else {
        for (i = 0; i < FLOW_CONTROL_MAP_SIZE; i++) {
            SendFlowControlQueue(fcm,  g_priorityMapTx[i], FLOW_TX);
        }
    }
}

static void FlowControlRxTreadProcess(struct FlowControlModule *fcm)
{
    bool isSta = false;
    int i;
    if (fcm->op != NULL && fcm->op->isDeviceStaOrP2PClient != NULL) {
        isSta = fcm->op->isDeviceStaOrP2PClient();
    }
    if (isSta) {
        for (i = 0; i < QUEUE_ID_COUNT; i++) {
            SendFlowControlQueue(fcm, g_staPriorityMapRx[i], FLOW_RX);
        }
    } else {
        for (i = 0; i < QUEUE_ID_COUNT; i++) {
            SendFlowControlQueue(fcm,  g_priorityMapRx[i], FLOW_RX);
        }
    }
}

static int32_t RunWiFiFlowControl(void *para, FlowDir dir)
{
    struct FlowControlModule *fcm = (struct FlowControlModule *)para;
    if (para == NULL || dir >= FLOW_DIR_COUNT) {
        HDF_LOGE("%s fail: para = null or dir=%d!", __func__, dir);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("%s Enter flow control dir =%d!", __func__, dir);
    fcm->threadStatus[dir] = THREAD_STARTING;
    while (true) {
        fcm->threadStatus[dir] = THREAD_WAITING;
        if (OsalSemWait(&fcm->sem[dir], HDF_WAIT_FOREVER) != HDF_SUCCESS) {
            HDF_LOGE("%s exit: OsalSemWait return false!", __func__);
            continue;
        }
        if (IsFcThreadNeedStop(fcm, dir)) {
            HDF_LOGE("%s exit: because threadStatus[%d] > THREAD_RUNNING!", __func__, dir);
            break;
        }
        fcm->threadStatus[dir] = THREAD_RUNNING;
        if (dir == FLOW_TX) {
            FlowControlTxTreadProcess(fcm);
        } else if (dir == FLOW_RX) {
            FlowControlRxTreadProcess(fcm);
        }
    }
    HDF_LOGE("%s Exit!", __func__);
    fcm->threadStatus[dir] = THREAD_STOPPED;
    return HDF_SUCCESS;
}

static int32_t RunWiFiTxFlowControl(void *para)
{
    return RunWiFiFlowControl(para, FLOW_TX);
}

static int32_t RunWiFiRxFlowControl(void *para)
{
    return RunWiFiFlowControl(para, FLOW_RX);
}

static int32_t CreateTask(struct OsalThread *thread, char *taskName, OsalThreadEntry threadEntry,
    struct OsalThreadParam *para, void *entryPara)
{
    int32_t status = OsalThreadCreate(thread, threadEntry, entryPara);
    para->name = taskName;
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s:OsalThreadCreate failed!status=%d", __func__, status);
        return HDF_FAILURE;
    }
    status = OsalThreadStart(thread, para);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s:OsalThreadStart failed!status=%d", __func__, status);
        OsalThreadDestroy(thread);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void DestroyTask(struct FlowControlModule *fcm, FlowDir dir)
{
    int count = 0;
    if (fcm == NULL || dir >= FLOW_DIR_COUNT) {
        HDF_LOGE("%s fail: fcm = null or dir not right!", __func__);
        return;
    }
    if (fcm->threadStatus[dir] == THREAD_INIT_FAIL || fcm->threadStatus[dir] == THREAD_DESTROYED) {
        HDF_LOGE("%s,delete thread not need!", __func__);
        return;
    }
    fcm->threadStatus[dir] = THREAD_STOPPING;
    OsalSemPost(&fcm->sem[dir]);

    /* wait until RunWiFiFlowControl exit */
    while ((fcm->threadStatus[dir] != THREAD_STOPPED) && (count < MAX_EXIT_THREAD_COUNT)) {
        OsalMSleep(1);
        count++;
    }
    if (dir == FLOW_TX) {
        OsalThreadDestroy(&fcm->txTransferThread);
    } else if (dir == FLOW_RX) {
        OsalThreadDestroy(&fcm->rxTransferThread);
    }
    fcm->threadStatus[dir] = THREAD_DESTROYED;
    HDF_LOGE("%s delete thread[%d] success!", __func__, dir);
    return;
}

int32_t CreateFlowControlTask(struct FlowControlModule *fcm)
{
    int32_t ret;
    struct OsalThreadParam config = {
        .priority = OSAL_THREAD_PRI_HIGHEST,
        .stackSize = 0,
    };

    if (fcm == NULL) {
        HDF_LOGE("%s fail: fcm = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CreateTask(&fcm->txTransferThread, RX_THREAD_NAME, RunWiFiTxFlowControl, &config, fcm);
    if (ret == HDF_FAILURE) {
        fcm->threadStatus[FLOW_TX] = THREAD_INIT_FAIL;
        return HDF_FAILURE;
    }
    ret = CreateTask(&fcm->rxTransferThread, TX_THREAD_NAME, RunWiFiRxFlowControl, &config, fcm);
    if (ret == HDF_FAILURE) {
        fcm->threadStatus[FLOW_TX] = THREAD_INIT_FAIL;
        fcm->threadStatus[FLOW_RX] = THREAD_INIT_FAIL;
        DestroyTask(fcm, FLOW_TX);
    }
    fcm->threadStatus[FLOW_TX] = THREAD_INIT_SUCCESS;
    fcm->threadStatus[FLOW_RX] = THREAD_INIT_SUCCESS;
    return HDF_SUCCESS;
}

void DestroyFlowControlTask(struct FlowControlModule *fcm)
{
    if (fcm == NULL) {
        HDF_LOGE("%s fcm = null", __func__);
        return;
    }
    DestroyTask(fcm, FLOW_TX);
    DestroyTask(fcm, FLOW_RX);
    return;
}