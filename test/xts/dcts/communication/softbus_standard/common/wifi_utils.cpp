/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "wifi_utils.h"
#include <arpa/inet.h>

using std::unique_ptr;
using namespace OHOS::Wifi;

const static int WIFI_SA_ID = 1125;
const static int TWO_SECOND = 2;
const static int FIVE_SECOND = 5;
static const char* def_ssid = "OpenHarmony_Private_Net_01";
static unique_ptr<WifiDevice> wifiDevicePtr = WifiDevice::GetInstance(WIFI_SA_ID);

int WiFiUtils::EnableWifi()
{
    bool isActive = false;
    int ret = wifiDevicePtr->IsWifiActive(isActive);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]call IsWifiActive fail, ret:%d", ret);
    }
    if (!isActive) {
        LOG("[wifi]not active, call enable...");
        ret = wifiDevicePtr->EnableWifi();
        if (ret != SOFTBUS_OK) {
            LOG("[wifi]call EnableWifi fail, ret:%d", ret);
            return SOFTBUS_ERR;
        }

        LOG("[wifi]call EnableWifi success, wait state active");
        int timeout = 10;
        do {
            ret = wifiDevicePtr->IsWifiActive(isActive);
            if (ret != SOFTBUS_OK) {
                LOG("[wifi]call IsWifiActive fail, ret:%d", ret);
            }
            if (isActive) {
                LOG("[wifi]is active, timeout:%d", timeout);
                break;
            }
            sleep(FIVE_SECOND);
            timeout--;
        } while (timeout > 0);

        if (timeout <= 0) {
            LOG("[wifi]wait wifi state fail[timeout]");
            return SOFTBUS_ERR;
        } else {
            LOG("[wifi]wait wifi state success");
            return SOFTBUS_OK;
        }
    } else {
        LOG("[wifi]is active, do nothing");
        return SOFTBUS_OK;
    }
}

int WiFiUtils::DisableWifi()
{
    bool isActive = false;
    int ret = wifiDevicePtr->IsWifiActive(isActive);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]call IsWifiActive fail, ret:%d", ret);
    }
    if (isActive) {
        LOG("[wifi]is active, call disable...");
        ret = wifiDevicePtr->DisableWifi();
        if (ret != SOFTBUS_OK) {
            LOG("[wifi]call DisableWifi fail, ret:%d", ret);
            return SOFTBUS_ERR;
        }

        LOG("[wifi]call DisableWifi success, wait state active");
        int timeout = 10;
        do {
            ret = wifiDevicePtr->IsWifiActive(isActive);
            if (ret != SOFTBUS_OK) {
                LOG("[wifi]call IsWifiActive fail, ret:%d", ret);
            }
            if (!isActive) {
                LOG("[wifi]not active, timeout:%d", timeout);
                break;
            }
            sleep(1);
            timeout--;
        } while (timeout > 0);

        if (timeout <= 0) {
            LOG("[wifi]wait wifi state fail[timeout]");
            return SOFTBUS_ERR;
        } else {
            LOG("[wifi]wait wifi state success");
            return SOFTBUS_OK;
        }
    } else {
        LOG("[wifi]not active, do nothing");
        return SOFTBUS_OK;
    }
}

int WiFiUtils::DisableThenEnable(int delaySeconds)
{
    int ret = DisableWifi();
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]DisableWifi fail");
        return ret;
    }
    sleep(delaySeconds);
    ret = EnableWifi();
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]EnableWifi fail");
    }
    return ret;
}

int WiFiUtils::CheckIsConnectToDefault(void)
{
    WifiLinkedInfo linkInfo;
    int ret = wifiDevicePtr->GetLinkedInfo(linkInfo);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]call GetLinkedInfo fail, ret:%d", ret);
    } else {
        char rstBuff[16];
        struct in_addr inputAddr = {0};
        inputAddr.s_addr = htonl(linkInfo.ipAddress);
        inet_ntop(AF_INET, &inputAddr, rstBuff, sizeof(rstBuff));
        LOG("[wifi]link info,netid:%d, ssid:%s, state:%d, ip:%u,%s",
            linkInfo.networkId, linkInfo.ssid.c_str(), linkInfo.connState,
            linkInfo.ipAddress, rstBuff);
        if (strncmp(linkInfo.ssid.c_str(), def_ssid, strlen(def_ssid)) == 0) {
            LOG("[wifi]check success");
            return SOFTBUS_OK;
        } else {
            LOG("[wifi]check fail");
        }
    }
    return SOFTBUS_ERR;
}

int WiFiUtils::ConnectTo(const std::string& ssid, const std::string& passwd)
{
    WifiDeviceConfig deviceConfig;
    deviceConfig.ssid = ssid;
    deviceConfig.preSharedKey = passwd;
    deviceConfig.keyMgmt = "WPA-PSK";
    int netId;
    int ret = wifiDevicePtr->AddDeviceConfig(deviceConfig, netId);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]call AddDeviceConfig fail, ret:%d", ret);
    } else {
        LOG("[wifi]call AddDeviceConfig success, netId:%d", netId);
    }

    ret = wifiDevicePtr->ConnectToNetwork(netId);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]call ConnectTo fail, ret:%d", ret);
        return SOFTBUS_ERR;
    } else {
        LOG("[wifi]call ConnectTo success, netId:%d", netId);
    }
    int timeout = 10;
    char rstBuff[16];
    WifiLinkedInfo linkInfo;
    struct in_addr inputAddr = {0};
    while (timeout > 0) {
        sleep(TWO_SECOND);
        ret = wifiDevicePtr->GetLinkedInfo(linkInfo);
        if (ret != SOFTBUS_OK) {
            LOG("[wifi]call GetLinkedInfo fail, ret:%d", ret);
        } else {
            LOG("[wifi]call GetLinkedInfo success, now state:%d",
                linkInfo.connState);
            if (linkInfo.connState == ConnState::CONNECTED &&
                linkInfo.ipAddress != 0) {
                inputAddr.s_addr = htonl(linkInfo.ipAddress);
                inet_ntop(AF_INET, &inputAddr, rstBuff, sizeof(rstBuff));
                LOG("[wifi]state OK,netid:%d, ssid:%s, state:%d, ip:%u,%s",
                    linkInfo.networkId, linkInfo.ssid.c_str(),
                    linkInfo.connState, linkInfo.ipAddress, rstBuff);
                break;
            }
        }
        timeout -= TWO_SECOND;
    }
    if (timeout == 0) {
        inputAddr.s_addr = htonl(linkInfo.ipAddress);
        inet_ntop(AF_INET, &inputAddr, rstBuff, sizeof(rstBuff));
        LOG("[wifi]state(timeout=0),netid:%d, ssid:%s, state:%d, ip:%u,%s",
            linkInfo.networkId, linkInfo.ssid.c_str(), linkInfo.connState,
            linkInfo.ipAddress, rstBuff);
    }
    return SOFTBUS_OK;
}

int WiFiUtils::ConnectToNew(const std::string& ssid,
                            const std::string& passwd)
{
    int ret = wifiDevicePtr->Disconnect();
    LOG("[wifi]call Disconnect ret:%d", ret);
    ret = ConnectTo(ssid, passwd);
    return ret;
}

int WiFiUtils::ConnectToOpenAP(const std::string& ssid)
{
    WifiDeviceConfig deviceConfig;
    deviceConfig.ssid = ssid;
    deviceConfig.keyMgmt = "NONE";
    int netId;
    int ret = wifiDevicePtr->AddDeviceConfig(deviceConfig, netId);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]call AddDeviceConfig fail, ret:%d", ret);
    } else {
        LOG("[wifi]call AddDeviceConfig success, netId:%d", netId);
    }

    ret = wifiDevicePtr->ConnectToNetwork(netId);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]call ConnectTo fail, ret:%d", ret);
        return SOFTBUS_ERR;
    } else {
        LOG("[wifi]call ConnectTo success, netId:%d", netId);
    }
    sleep(FIVE_SECOND);
    int state;
    ret = wifiDevicePtr->GetWifiState(state);
    LOG("[wifi]call GetWifiState ret:%d,state:%d", ret, state);

    WifiLinkedInfo linkInfo;
    ret = wifiDevicePtr->GetLinkedInfo(linkInfo);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]call GetLinkedInfo fail, ret:%d", ret);
    } else {
        LOG("[wifi]call GetLinkedInfo success");
        char rstBuff[16];
        struct in_addr inputAddr = {0};
        inputAddr.s_addr = htonl(linkInfo.ipAddress);
        inet_ntop(AF_INET, &inputAddr, rstBuff, sizeof(rstBuff));

        LOG("[wifi]link info,netid:%d, ssid:%s, state:%d, ip:%u,%s",
            linkInfo.networkId, linkInfo.ssid.c_str(), linkInfo.connState,
            linkInfo.ipAddress, rstBuff);
    }
    return SOFTBUS_OK;
}

int WiFiUtils::EnableThenConnect(const std::string& ssid,
                                 const std::string& passwd)
{
    int ret = EnableWifi();
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]EnableWifi fail");
        return ret;
    }
    ret = ConnectTo(ssid, passwd);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]ConnectTo fail");
    }
    return ret;
}

int WiFiUtils::DisableThenEnableAndConnect(int delaySeconds,
                                           const std::string& ssid,
                                           const std::string& passwd)
{
    int ret = DisableWifi();
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]DisableWifi fail");
        return ret;
    }
    sleep(delaySeconds);
    ret = EnableWifi();
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]EnableWifi fail");
        return ret;
    }
    ret = ConnectTo(ssid, passwd);
    if (ret != SOFTBUS_OK) {
        LOG("[wifi]ConnectTo fail");
    }
    return ret;
}
