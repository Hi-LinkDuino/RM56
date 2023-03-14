/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HDF_WIFI_TEST_H
#define HDF_WIFI_TEST_H

#include <cstdint>

// wifi test case number
enum HdfWiFiTestCaseCmd {
    /* flow control. */
    WIFI_FLOW_CONTROL_INIT,
    WIFI_FLOW_CONTROL_DEINIT,
    WIFI_FLOW_CONTROL_GET_QUEUE_ID,
    WIFI_FLOW_CONTROL_SEND_DATA,
    WIFI_FLOW_CONTROL_END = 50,
    /* netdevice. */
    WIFI_NET_DEVICE_INIT = WIFI_FLOW_CONTROL_END,
    WIFI_NET_DEVICE_DEINIT,
    WIFI_NET_DEVICE_ADD,
    WIFI_NET_DEVICE_DELETE,
    WIFI_NET_DEVICE_GET,
    WIFI_NET_DEVICE_GET_COUNT,
    WIFI_NET_DEVICE_GET_CAP,
    WIFI_NET_DEVICE_SET_ADDR,
    WIFI_NET_DEVICE_SET_STATUS,
    WIFI_NET_DEVICE_SET_LINK_STATUS,
    WIFI_NET_DEVICE_RX,
    WIFI_NET_DEVICE_DHCPC,
    WIFI_NET_DEVICE_DHCPS,
    WIFI_NET_DEVICE_END = 100,
    /* netbuff */
    WIFI_NET_BUF_TEST = WIFI_NET_DEVICE_END,
    WIFI_NET_BUF_QUEUE_TEST,
    WIFI_NET_BUFF_END = 150,
    /* module */
    WIFI_MODULE_CREATE_MODULE = WIFI_NET_BUFF_END,
    WIFI_MODULE_DELETE_MODULE,
    WIFI_MODULE_ADD_FEATURE,
    WIFI_MODULE_DELETE_FEATURE,
    WIFI_MODULE_END = 200,
    /* message */
    WIFI_MESSAGE_QUEUE_001 = WIFI_MODULE_END,
    WIFI_MESSAGE_QUEUE_002,
    WIFI_MESSAGE_QUEUE_003,
    WIFI_MESSAGE_SINGLE_NODE_001,
    WIFI_MESSAGE_SINGLE_NODE_002,
    WIFI_MESSAGE_SINGLE_NODE_003,
    WIFI_MESSAGE_SINGLE_NODE_004,
    WIFI_MESSAGE_SINGLE_NODE_005,
    WIFI_MESSAGE_END = 300,
};

#endif // HDF_WIFI_TEST_H
