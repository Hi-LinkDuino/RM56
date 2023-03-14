/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "flow_control_test.h"
#include "flow_control.h"
#include "hdf_log.h"
#include "securec.h"
#include "osal_time.h"

#define WATITE_RESULT_TIME 1000 // 1000ms
static struct FlowControlModule *g_flowControlInstance = NULL;
static bool g_result = false;

static uint8_t g_dhcpData[] = {
    0x42, 0x2b, 0x13, 0x41, 0xc9, 0xd7, 0x38, 0x81, 0x13, 0x1c, 0x6d, 0xad, 0x08, 0x00, 0x45, 0x00, 0x01, 0x50, 0x00,
    0x01, 0x00, 0x00, 0xff, 0x11, 0xba, 0x9c, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x44, 0x00, 0x43,
    0x01, 0x3c, 0xd4, 0xa8
};

static bool IsDeviceStaOrP2PClient(void)
{
    return true;
}

static int32_t SendDataPacket(NetBufQueue *q, void *fcmPrivate, int32_t fwPriorityId)
{
    NetBuf *buff = NULL;
    uint32_t count = NetBufQueueSize(q);
    if (count != 1) {
        HDF_LOGE("%s count error", __func__);
        return HDF_FAILURE;
    }
    buff = NetBufQueueDequeue(q);
    g_result = false;
    HDF_LOGE("%s enter", __func__);
    if (memcmp(NetBufGetAddress(buff, E_DATA_BUF), g_dhcpData, sizeof(g_dhcpData) / sizeof(g_dhcpData[0])) != 0) {
        HDF_LOGE("%s fail : memcmp != size", __func__);
        return HDF_FAILURE;
    }
    g_result = true;
    (void)fcmPrivate;
    (void)fwPriorityId;
    return HDF_SUCCESS;
}

static struct FlowControlOp g_flowControlOp = {
    .isDeviceStaOrP2PClient = IsDeviceStaOrP2PClient,
    .txDataPacket = SendDataPacket,
    .rxDataPacket = NULL,
    .getTxQueueId = NULL,
    .getRxQueueId = NULL,
    .getTxPriorityId = NULL,
    .getRxPriorityId = NULL,
};

static bool WiFiFlowControlTestEnv(void)
{
    if (g_flowControlInstance == NULL) {
        g_flowControlInstance = InitFlowControl(NULL);
        if (g_flowControlInstance == NULL) {
            HDF_LOGE("%s fail : InitFlowControl = null!", __func__);
            return false;
        }
    }
    g_flowControlInstance->interface->registerFlowControlOp(g_flowControlInstance, &g_flowControlOp);
    return true;
}

static bool WiFiFlowControlTestOut(void)
{
    if (g_flowControlInstance != NULL) {
        DeInitFlowControl(g_flowControlInstance);
        g_flowControlInstance = NULL;
    }
    return true;
}

static NetBuf *ConstructEapolNetBuf(void)
{
    NetBuf *buff = NULL;
    int count = sizeof(g_dhcpData) / sizeof(g_dhcpData[0]);
    buff = NetBufAlloc(count);
    if (buff == NULL) {
        HDF_LOGE("%s fail : NetBufAlloc = null!", __func__);
        return buff;
    }
    NetBufPush(buff, E_DATA_BUF, count);
    if (memcpy_s(NetBufGetAddress(buff, E_DATA_BUF), count, g_dhcpData, count) != EOK) {
        NetBufFree(buff);
        buff = NULL;
        HDF_LOGE("%s fail : memcpy_s fail", __func__);
        return buff;
    }
    return buff;
}

int32_t WiFiFlowControlTestInit(void)
{
    if (WiFiFlowControlTestEnv()) {
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}

int32_t WiFiFlowControlTestDeinit(void)
{
    if (WiFiFlowControlTestOut()) {
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}

int32_t WiFiFlowControlTestGetEapolQueueId(void)
{
    NetBuf *buff = NULL;
    FlowControlQueueID id;
    if (!WiFiFlowControlTestEnv()) {
        return HDF_FAILURE;
    }
    buff = ConstructEapolNetBuf();
    if (buff == NULL) {
        return HDF_FAILURE;
    }
    if (g_flowControlInstance->interface == NULL) {
        HDF_LOGE("%s interface =null!", __func__);
        return HDF_FAILURE;
    }
    id = g_flowControlInstance->interface->getQueueIdByEtherBuff(buff);
    HDF_LOGE("%s get id =%d!", __func__, id);

    NetBufFree(buff);
    DeInitFlowControl(g_flowControlInstance);
    g_flowControlInstance = NULL;
    if (id == VIP_QUEUE_ID) {
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}

int32_t WiFiFlowControlTestSendData(void)
{
    NetBuf *buff = NULL;
    FlowControlQueueID id;
    if (!WiFiFlowControlTestEnv()) {
        return false;
    }
    buff = ConstructEapolNetBuf();
    if (buff == NULL) {
        return HDF_FAILURE;
    }
    if (g_flowControlInstance->interface == NULL) {
        HDF_LOGE("%s interface = null!", __func__);
        return HDF_FAILURE;
    }
    id = g_flowControlInstance->interface->getQueueIdByEtherBuff(buff);
    g_result = false;
    if (g_flowControlInstance->interface->sendBuffToFCM(g_flowControlInstance, buff, id, FLOW_TX) != HDF_SUCCESS) {
        HDF_LOGE("%s sendBuffToFCM fail!", __func__);
        return HDF_FAILURE;
    }
    if (g_flowControlInstance->interface->schedFCM(g_flowControlInstance, FLOW_TX) != HDF_SUCCESS) {
        HDF_LOGE("%s sendBuffToFCM fail!", __func__);
        return HDF_FAILURE;
    }
    OsalMSleep(WATITE_RESULT_TIME);
    NetBufFree(buff);
    DeInitFlowControl(g_flowControlInstance);
    g_flowControlInstance = NULL;
    HDF_LOGE("%s g_result = %d!", __func__, g_result);
    return g_result ? HDF_SUCCESS : HDF_FAILURE;
}