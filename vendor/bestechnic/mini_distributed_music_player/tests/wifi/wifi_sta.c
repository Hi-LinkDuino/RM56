/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "wifi_device.h"
#include "wifi_error_code.h"
#include <stdlib.h>
#include <string.h>

#define SELECT_WIFI_SSID "test_wifi"
#define SELECT_WIFI_PASSWORD  "12345678"
#define SELECT_WIFI_SECURITYTYPE WIFI_SEC_TYPE_PSK

#define SCAN_OPTION 0
#if SCAN_OPTION
#define SCAN_SUCCESS_FLAGS 1U
osEventFlagsId_t eventId;
#endif
WifiEvent g_wifiEventHandler = {0};
WifiErrorCode error;

static void OnWifiScanStateChangedHandler(int state, int size)
{
    (void)state;
    if (state > 0) {
#if SCAN_OPTION
        osEventFlagsSet(eventId, SCAN_SUCCESS_FLAGS);
#endif
        printf("wifi scan result %d\r\n", size);
    } else {
        printf("wifi scan failed\r\n");
    }
}

static void OnWifiConnectionChangedHandler(int state, WifiLinkedInfo *info)
{
    (void)info;
    if (state > 0) {
        printf("callback function for wifi connect\r\n");
    } else {
        printf("connect error,please check password\r\n");
    }
}

static void OnHotspotStaJoinHandler(StationInfo *info)
{
    (void)info;
    printf("STA join AP\n");
}

static void OnHotspotStaLeaveHandler(StationInfo *info)
{
    (void)info;
    printf("HotspotStaLeave:info is null.\n");
}

static void OnHotspotStateChangedHandler(int state)
{
    printf("HotspotStateChanged:state is %d.\n", state);
}

static WifiErrorCode WiFiInit(void)
{
    g_wifiEventHandler.OnWifiScanStateChanged = OnWifiScanStateChangedHandler;
    g_wifiEventHandler.OnWifiConnectionChanged = OnWifiConnectionChangedHandler;
    g_wifiEventHandler.OnHotspotStaJoin = OnHotspotStaJoinHandler;
    g_wifiEventHandler.OnHotspotStaLeave = OnHotspotStaLeaveHandler;
    g_wifiEventHandler.OnHotspotStateChanged = OnHotspotStateChangedHandler;
    return RegisterWifiEvent(&g_wifiEventHandler);
}

#if SCAN_OPTION
static int WifiScan(uint32_t timeoutMs)
{
    uint32_t flags = 0;
    if (Scan() == ERROR_WIFI_IFACE_INVALID)
        return -1;

    flags = osEventFlagsWait(eventId, SCAN_SUCCESS_FLAGS, osFlagsWaitAny, timeoutMs);
    return (flags == SCAN_SUCCESS_FLAGS) ? 0 : -1;
}
#endif

static void WifiSTATask(void)
{
    WifiDeviceConfig select_ap_config = {0};
    strcpy(select_ap_config.ssid, SELECT_WIFI_SSID);
    strcpy(select_ap_config.preSharedKey, SELECT_WIFI_PASSWORD);
    select_ap_config.securityType = SELECT_WIFI_SECURITYTYPE;

    osDelay(2000);
    printf("<--WifiSTATask Init-->\r\n");

    if (WiFiInit() != WIFI_SUCCESS) {
        printf("WiFiInit failed!\r\n");
        return;
    }

    if (EnableWifi() != WIFI_SUCCESS) {
        printf("EnableWifi failed\r\n");
        return;
    }

    if (IsWifiActive() == WIFI_STA_NOT_ACTIVE) {
        printf("Wifi station is not actived.\n");
        return;
    }

#if SCAN_OPTION
    eventId = osEventFlagsNew(NULL);
    if (eventId == NULL) {
        printf("Falied to create EventFlags!\n");
        return;
    }
    if (WifiScan(10000) < 0) {
        printf("WifiScan failed\r\n");
        return;
    }
    unsigned int size = WIFI_SCAN_HOTSPOT_LIMIT;
    WifiScanInfo *info = (WifiScanInfo *)malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);
    if (info == NULL) {
        printf("malloc failed\r\n");
        return;
    }
    if (GetScanInfoList(info, &size) != WIFI_SUCCESS) ///< size = scan_result_len
    {
        printf("GetScanInfoList failed\r\n");
        free(info);
        return;
    }
    printf("********************\r\n");
    for (unsigned int i = 0; i < size; i++) {
        printf("no:%03u, ssid:%-30s, rssi:%5d\r\n", i + 1, info[i].ssid, info[i].rssi);
    }
    printf("********************\r\n");
    free(info);
#endif

    int result;
    if (AddDeviceConfig(&select_ap_config, &result) != WIFI_SUCCESS) {
        printf("AddDeviceConfig failed!\r\n");
        return;
    }
    printf("Connecting to %s...\r\n", SELECT_WIFI_SSID);
    error = ConnectTo(result); ///< block and retry
    printf("WiFi connect %s!\r\n", (error == WIFI_SUCCESS) ? "succeed" : "failed");

    for (;;) {
        osDelay(100);
    }
}

static void WifiClientSTA(void)
{
    printf("[%s:%d]: %s\n", __FILE__, __LINE__, __func__);

    osThreadAttr_t attr;
    attr.name = "WifiSTATask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = 24;

    if (osThreadNew((osThreadFunc_t)WifiSTATask, NULL, &attr) == NULL) {
        printf("Falied to create WifiSTATask!\n");
    }
}

APP_FEATURE_INIT(WifiClientSTA);
