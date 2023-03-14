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

#include "wifi_event.h"
#include "station_info.h"
#include "wifi_scan_info.h"
#include "wifi_error_code.h"
#include "wifi_linked_info.h"
#include "wifi_device_config.h"
#include "bwifi_interface.h"
#include "wifi_hotspot_config.h"
#include "bwifi_event.h"

#define RSSI_LEVEL_4_2_G (-65)
#define RSSI_LEVEL_3_2_G (-75)
#define RSSI_LEVEL_2_2_G (-82)
#define RSSI_LEVEL_1_2_G (-88)
#define RSSI_LEVEL_4_5_G (-65)
#define RSSI_LEVEL_3_5_G (-72)
#define RSSI_LEVEL_2_5_G (-79)
#define RSSI_LEVEL_1_5_G (-85)


static int g_HalHmosWifiApStatus                = WIFI_HOTSPOT_NOT_ACTIVE;
static HotspotConfig g_HalHmosWifiApConfig      = {0};
extern WifiErrorCode HalHmosWifiEventInit();
extern void HalHmosWifiLock();
extern void HalHmosWifiUnLock();

static int HalBesSecTypeConvert(WifiSecurityType security_type)
{
    switch (security_type) {
    case WIFI_SEC_TYPE_WEP:
        return BWIFI_SECURITY_WEP104;
    case WIFI_SEC_TYPE_PSK:
        return BWIFI_SECURITY_WPA_WPA2;
    case WIFI_SEC_TYPE_SAE:
        return BWIFI_SECURITY_WPA3_SAE_WPA2;
    default:
        return WIFI_SEC_TYPE_OPEN;
    }
}

WifiErrorCode EnableHotspot(void)
{
    WifiErrorCode ret           = ERROR_WIFI_UNKNOWN;
    BWIFI_SEC_TYPE_T sectype    = HalBesSecTypeConvert(g_HalHmosWifiApConfig.securityType);
    HalHmosWifiLock();
    if (BWIFI_R_OK == bwifi_set_softap_config((char *)g_HalHmosWifiApConfig.ssid, 
                        g_HalHmosWifiApConfig.channelNum, 0, 0,
                        sectype, (char *)g_HalHmosWifiApConfig.preSharedKey)) {
        if (g_HalHmosWifiApStatus != WIFI_HOTSPOT_ACTIVE
            &&HalHmosWifiEventInit() == WIFI_SUCCESS) {
            if (bwifi_softap_start() == BWIFI_R_OK) {
                g_HalHmosWifiApStatus = WIFI_HOTSPOT_ACTIVE;
                ret = WIFI_SUCCESS;
            }
        }
    }
    HalHmosWifiUnLock();
    return ret;
}

WifiErrorCode DisableHotspot(void)
{
    HalHmosWifiLock();
    if(g_HalHmosWifiApStatus == WIFI_HOTSPOT_NOT_ACTIVE)
        return ERROR_WIFI_NOT_STARTED;
    bwifi_softap_stop();
    g_HalHmosWifiApStatus = WIFI_HOTSPOT_NOT_ACTIVE;
    HalHmosWifiUnLock();
    return WIFI_SUCCESS;
}

WifiErrorCode SetHotspotConfig(const HotspotConfig *config)
{
    if (config == NULL)
        return ERROR_WIFI_INVALID_ARGS;
    HalHmosWifiLock();
    memcpy(&g_HalHmosWifiApConfig, config, sizeof(HotspotConfig));
    HalHmosWifiUnLock();
    return WIFI_SUCCESS;
}

WifiErrorCode GetHotspotConfig(HotspotConfig *result)
{
    if (result == NULL)
        return ERROR_WIFI_INVALID_ARGS;
    HalHmosWifiLock();
    memcpy(result, &g_HalHmosWifiApConfig, sizeof(HotspotConfig));
    HalHmosWifiUnLock();
    return WIFI_SUCCESS;
}

int IsHotspotActive(void)
{
    return g_HalHmosWifiApStatus;
}

WifiErrorCode GetStationList(StationInfo *result, unsigned int *size)
{

    int i                               = 0;
    int count                           = *size;
    WifiErrorCode ret                   = ERROR_WIFI_INVALID_ARGS;
    struct bwifi_mac_addr *mac_list     = NULL;

    if (result == NULL)
        return ret;

    mac_list = (struct bwifi_mac_addr *)malloc(count * sizeof(struct bwifi_mac_addr));
    if (mac_list != NULL) {
        HalHmosWifiLock();
        if (bwifi_softap_get_station_list(mac_list, &count) == BWIFI_R_OK) {
            for (i = 0; i < count; i++) {
                memcpy((uint8_t *)result->macAddress, mac_list++, WIFI_MAC_LEN);
                result->disconnectedReason = 0;
                result++;
            }
            *size = count;
        } else
            *size = 0;

        HalHmosWifiUnLock();
        free(mac_list);
    }

    return WIFI_SUCCESS;

}


int GetSignalLevel(int rssi, int band)
{
    if (band == HOTSPOT_BAND_TYPE_2G) {
        if (rssi >= RSSI_LEVEL_4_2_G)
            return RSSI_LEVEL_4;
        if (rssi >= RSSI_LEVEL_3_2_G)
            return RSSI_LEVEL_3;
        if (rssi >= RSSI_LEVEL_2_2_G)
            return RSSI_LEVEL_2;
        if (rssi >= RSSI_LEVEL_1_2_G)
            return RSSI_LEVEL_1;
    }

    if (band == HOTSPOT_BAND_TYPE_5G) {
        if (rssi >= RSSI_LEVEL_4_5_G)
            return RSSI_LEVEL_4;
        if (rssi >= RSSI_LEVEL_3_5_G)
            return RSSI_LEVEL_3;
        if (rssi >= RSSI_LEVEL_2_5_G)
            return RSSI_LEVEL_2;
        if (rssi >= RSSI_LEVEL_1_5_G)
            return RSSI_LEVEL_1;
    }
    return ERROR_WIFI_INVALID_ARGS;
}

WifiErrorCode SetBand(int band)
{
    if(band != 1 && band != 2)
        return ERROR_WIFI_NOT_SUPPORTED;
    g_HalHmosWifiApConfig.band = band;
    return WIFI_SUCCESS;
}

WifiErrorCode GetBand(int *result)
{
    if(g_HalHmosWifiApConfig.band == 0)
        if(*result != 1 && *result != 2)
            return ERROR_WIFI_NOT_SUPPORTED;
    *result = g_HalHmosWifiApConfig.band;
    return WIFI_SUCCESS;
}


