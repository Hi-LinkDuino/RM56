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
#include "hal_trace.h"
#include "ohos_init.h"
#include "wifi_device.h"
#include "wifi_error_code.h"
#include "wifi_hotspot.h"
#include <string.h>
#include <unistd.h>

#define AP_SSID "test_wifi"
#define AP_PSK "12345678"

static void OnHotspotStaJoinHandler(StationInfo *info);
static void OnHotspotStateChangedHandler(int state);
static void OnHotspotStaLeaveHandler(StationInfo *info);

static int g_apEnableSuccess = 0;
WifiEvent g_wifiEventHandler = {0};
WifiErrorCode error;

static void WifiAPTask(void)
{
    osDelay(2000);

    //注册wifi事件的回调函数
    g_wifiEventHandler.OnHotspotStaJoin = OnHotspotStaJoinHandler;
    g_wifiEventHandler.OnHotspotStaLeave = OnHotspotStaLeaveHandler;
    g_wifiEventHandler.OnHotspotStateChanged = OnHotspotStateChangedHandler;
    error = RegisterWifiEvent(&g_wifiEventHandler);
    if (error != WIFI_SUCCESS) {
        printf("RegisterWifiEvent failed, error = %d.\r\n", error);
        return;
    }
    printf("RegisterWifiEvent succeed!\r\n");

    HotspotConfig config = {0};
    strcpy(config.ssid, AP_SSID);
    strcpy(config.preSharedKey, AP_PSK);
    config.securityType = WIFI_SEC_TYPE_PSK;
    config.band = HOTSPOT_BAND_TYPE_2G;
    config.channelNum = 7;

    error = SetHotspotConfig(&config);
    if (error != WIFI_SUCCESS) {
        printf("SetHotspotConfig failed, error = %d.\r\n", error);
        return;
    }
    printf("SetHotspotConfig succeed!\r\n");

    error = EnableHotspot();
    if (error != WIFI_SUCCESS) {
        printf("EnableHotspot failed, error = %d.\r\n", error);
        return -1;
    }
    printf("EnableHotspot succeed!\r\n");

    if (IsHotspotActive() == WIFI_HOTSPOT_NOT_ACTIVE) {
        printf("Wifi Hotspot is not actived.\r\n");
        return -1;
    }
    printf("Wifi Hotspot is actived!\r\n");
}

static void OnHotspotStaJoinHandler(StationInfo *info)
{
    if (info == NULL) {
        printf("HotspotStaJoin:info is null.\r\n");
        return;
    }
    StationInfo stainfo[WIFI_MAX_STA_NUM] = {0};
    StationInfo *sta_list_node = NULL;
    unsigned int size = WIFI_MAX_STA_NUM;

    error = GetStationList(stainfo, &size);
    if (error != WIFI_SUCCESS) {
        printf("HotspotStaJoin:get list fail, error is %d.\r\n", error);
        return;
    }
    sta_list_node = stainfo;
    for (uint32_t i = 0; i < size; i++, sta_list_node++) {
        unsigned char *mac = sta_list_node->macAddress;
        printf("HotspotSta[%u]: macAddress=%02X:%02X:%02X:%02X:%02X:%02X\r\n", i,
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    g_apEnableSuccess++;
}

static void OnHotspotStaLeaveHandler(StationInfo *info)
{
    if (info == NULL) {
        printf("HotspotStaLeave:info is null.\r\n");
        return;
    }
    unsigned char *mac = info->macAddress;
    printf("HotspotStaLeave: macAddress=%02X:%02X:%02X:%02X:%02X:%02X, reason=%d.\r\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
           info->disconnectedReason);
    g_apEnableSuccess--;
}

static void OnHotspotStateChangedHandler(int state)
{
    printf("HotspotStateChanged:state is %d.\r\n", state);
    if (state == WIFI_HOTSPOT_ACTIVE) {
        printf("wifi hotspot active.\r\n");
    } else {
        printf("wifi hotspot noactive.\r\n");
    }
}

static void Wifi_AP_Demo(void)
{
    printf("[%s:%d]: %s\n", __FILE__, __LINE__, __func__);

    osThreadAttr_t attr;
    attr.name = "WifiAPTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t)WifiAPTask, NULL, &attr) == NULL) {
        printf("Falied to create WifiAPTask!\r\n");
    }
}

APP_FEATURE_INIT(Wifi_AP_Demo);
