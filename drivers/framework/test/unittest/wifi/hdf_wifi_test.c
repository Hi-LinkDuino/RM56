/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_wifi_test.h"
#include "flow_control_test.h"
#include "hdf_module_test.h"
#include "hdf_log.h"
#include "hdf_message_test.h"
#include "hdf_netbuf_test.h"
#include "net_device_test.h"
#include "osal.h"

// add test case entry
static HdfTestCaseList g_hdfWiFiTestCaseList[] = {
    {WIFI_NET_DEVICE_INIT, WiFiNetDviceTestInit},
    {WIFI_NET_DEVICE_DEINIT, WiFiNetDviceTestDeInit},
    {WIFI_NET_DEVICE_ADD, WiFiNetDviceTestAdd},
    {WIFI_NET_DEVICE_DELETE, WiFiNetDviceTestDelete},
    {WIFI_NET_DEVICE_GET, WiFiNetDviceTestGetDevice},
    {WIFI_NET_DEVICE_GET_COUNT, WiFiNetDviceTestGetCount},
    {WIFI_NET_DEVICE_GET_CAP, WiFiNetDviceTestGetCap},
    {WIFI_NET_DEVICE_SET_ADDR, WiFiNetDviceTestSetAddr},
    {WIFI_NET_DEVICE_SET_STATUS, WiFiNetDviceTestSetStatus},
    {WIFI_NET_DEVICE_SET_LINK_STATUS, WiFiNetDviceTestSetLinkStatus},
    {WIFI_NET_DEVICE_RX, WiFiNetDviceTestRx},
    {WIFI_NET_DEVICE_DHCPC, WifiNetDeviceDhcpClient},
    {WIFI_NET_DEVICE_DHCPS, WifiNetDeviceDhcpServer},
    {WIFI_NET_BUF_TEST, HdfNetBufTest},
    {WIFI_NET_BUF_QUEUE_TEST, HdfNetBufQueueTest},
    {WIFI_MODULE_CREATE_MODULE, WiFiModuleTestCreateModule},
    {WIFI_MODULE_DELETE_MODULE, WiFiModuleTestDeleteModule},
    {WIFI_MODULE_ADD_FEATURE, WiFiModuleTestAddFeature},
    {WIFI_MODULE_DELETE_FEATURE, WiFiModuleTestDelFeature},
    {WIFI_FLOW_CONTROL_INIT, WiFiFlowControlTestInit},
    {WIFI_FLOW_CONTROL_DEINIT, WiFiFlowControlTestDeinit},
    {WIFI_FLOW_CONTROL_GET_QUEUE_ID, WiFiFlowControlTestGetEapolQueueId},
    {WIFI_FLOW_CONTROL_SEND_DATA, WiFiFlowControlTestSendData},
    {WIFI_MESSAGE_QUEUE_001, MessageQueueTest001},
    {WIFI_MESSAGE_QUEUE_002, MessageQueueTest002},
    {WIFI_MESSAGE_QUEUE_003, MessageQueueTest003},
    {WIFI_MESSAGE_SINGLE_NODE_001, MessageSingleNodeTest001},
    {WIFI_MESSAGE_SINGLE_NODE_002, MessageSingleNodeTest002},
    {WIFI_MESSAGE_SINGLE_NODE_003, MessageSingleNodeTest003},
    {WIFI_MESSAGE_SINGLE_NODE_004, MessageSingleNodeTest004},
    {WIFI_MESSAGE_SINGLE_NODE_005, MessageSingleNodeTest005},
};

int32_t HdfWifiEntry(HdfTestMsg *msg)
{
    int32_t result, i;

    if (msg == NULL) {
        HDF_LOGE("%s is fail: HdfTestMsg is NULL!", __func__);
        return HDF_SUCCESS;
    }

    for (i = 0; i < sizeof(g_hdfWiFiTestCaseList) / sizeof(g_hdfWiFiTestCaseList[0]); ++i) {
        if ((msg->subCmd == g_hdfWiFiTestCaseList[i].subCmd) && (g_hdfWiFiTestCaseList[i].testFunc != NULL)) {
            result = g_hdfWiFiTestCaseList[i].testFunc();
            HDF_LOGE("HdfTest:Wifi test result[%s-%u]", ((result == 0) ? "pass" : "fail"), msg->subCmd);
            msg->result = (result == 0) ? HDF_SUCCESS : HDF_FAILURE;
            return HDF_SUCCESS;
        }
    }
    return HDF_SUCCESS;
}