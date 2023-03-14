/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "wifiaware.h"
#include <string.h>
#include "hal_wifiaware.h"
#include "wifi_device_util.h"
#include "wifi_hotspot.h"

#define WIFIAWARE_TX_LOW_POWER (-61)
#define SHA256_BYTE_LEN 32
#define WIFIAWARE_MAX_POWER (-42)
#define WIFIAWARE_MIN_POWER (-70)
static signed char g_power = WIFIAWARE_TX_LOW_POWER;

int InitNAN(void)
{
    int ret;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};

    if (IsHotspotActive() != WIFI_HOTSPOT_ACTIVE) {
        return WIFIAWARE_FAIL;
    }

    if (GetHotspotChannel() != WIFIAWARE_DEFAULT_CHANNEL) {
        return WIFIAWARE_FAIL;
    }

    ret = GetHotspotInterfaceName(ifname, sizeof(ifname));
    if (ret != WIFI_SUCCESS) {
        return WIFIAWARE_FAIL;
    }

    if (HalWifiSdpInit(ifname) != 0) {
        return WIFIAWARE_FAIL;
    }

    return WIFIAWARE_SUCCESS;
}

int SubscribeService(const char* svcName, unsigned char localHandle, RecvCallback recvCB)
{
    int svcNameLen;
    unsigned char shaSvcName[SHA256_BYTE_LEN] = {0};

    if (svcName == 0 || recvCB == 0) {
        return WIFIAWARE_FAIL;
    }

    svcNameLen = strlen(svcName);
    if (svcNameLen == 0) {
        return WIFIAWARE_FAIL;
    }

    if (HalCipherHashSha256(svcName, svcNameLen, shaSvcName, sizeof(shaSvcName)) != 0) {
        return WIFIAWARE_FAIL;
    }

    if (HalWifiSdpStartService((char*)shaSvcName, localHandle, (HalRecvCallback)recvCB, HAL_WIFI_SDP_SUBSCRIBE) != 0) {
        return WIFIAWARE_FAIL;
    }

    return WIFIAWARE_SUCCESS;
}

int SendData(unsigned char* macAddr, unsigned char peerHandle, unsigned char localHandle,
    unsigned char* msg, int len)
{
    if (HalWifiSdpSend(macAddr, peerHandle, localHandle, msg, len) != 0) {
        return WIFIAWARE_FAIL;
    }

    return WIFIAWARE_SUCCESS;
}

int StopSubscribe(unsigned char localHandle)
{
    if (HalWifiSdpStopService(localHandle, HAL_WIFI_SDP_SUBSCRIBE) != 0) {
        return WIFIAWARE_FAIL;
    }

    return WIFIAWARE_SUCCESS;
}

int DeinitNAN(void)
{
    if (HalWifiSdpDeinit() != 0) {
        return WIFIAWARE_FAIL;
    }

    return WIFIAWARE_SUCCESS;
}

int SetLowPower(int powerSwitch)
{
    int ret;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};

    ret = GetHotspotInterfaceName(ifname, sizeof(ifname));
    if (ret != WIFI_SUCCESS) {
        return WIFIAWARE_FAIL;
    }

    switch (powerSwitch) {
        case WIFIAWARE_LOW_POWER_SWITCH_ON:
            if (HalWifiSdpAdjustTxPower(ifname, g_power) != 0) {
                return WIFIAWARE_FAIL;
            }
            break;
        case WIFIAWARE_LOW_POWER_SWITCH_OFF:
            if (HalWifiSdpRestoreTxPower(ifname) != 0) {
                return WIFIAWARE_FAIL;
            }
            break;
        default:
            return WIFIAWARE_FAIL;
    }

    return WIFIAWARE_SUCCESS;
}

int SetPower(signed char value)
{
    g_power = value;
    return WIFIAWARE_SUCCESS;
}

char GetPower(void)
{
    return g_power;
}

int NanBeaconSwitch(unsigned char enable)
{
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};

    if (GetHotspotInterfaceName(ifname, sizeof(ifname)) != WIFI_SUCCESS) {
        return WIFIAWARE_FAIL;
    }
    if (HalWifiSdpBeaconSwitch(ifname, enable) != 0) {
        return WIFIAWARE_FAIL;
    }

    return WIFIAWARE_SUCCESS;
}

int NanSetRetryTimes(unsigned int retries)
{
    if (HalWifiSdpSetRetryTimes(retries) != 0) {
        return WIFIAWARE_FAIL;
    }

    return WIFIAWARE_SUCCESS;
}

int NanGetSyncMode(void)
{
    return HalWifiSdpGetSyncMode();
}
