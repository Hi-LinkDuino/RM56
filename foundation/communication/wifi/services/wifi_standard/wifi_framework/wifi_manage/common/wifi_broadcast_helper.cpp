/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "wifi_broadcast_helper.h"
#include <stdio.h>
#include <vector>
#include "wifi_logger.h"

DEFINE_WIFILOG_LABEL("WifiBroadCastHelper");

namespace OHOS {
namespace Wifi {
void WifiBroadCastHelper::Show(char v)
{
    WIFI_LOGI("    -c:%{public}c", v);
    return;
}

void WifiBroadCastHelper::Show(const std::string &v)
{
    WIFI_LOGI("    -str:%{public}s", v.c_str());
    return;
}

void WifiBroadCastHelper::Show(int v)
{
    WIFI_LOGI("    -i:%{public}d", v);
    return;
}

void WifiBroadCastHelper::Show(const WifiP2pDevice &v)
{
    std::string rollpri;
    WIFI_LOGI("    -WifiP2pDevice:");
    WIFI_LOGI("       DeviceName:%{public}s", v.GetDeviceName().c_str());
    WIFI_LOGI("       DeviceAddress:%{private}s", v.GetDeviceAddress().c_str());
    WIFI_LOGI("       PrimaryDeviceType:%{public}s", v.GetPrimaryDeviceType().c_str());
    WIFI_LOGI("       SecondaryDeviceType:%{public}s", v.GetSecondaryDeviceType().c_str());
    WIFI_LOGI("       P2pDeviceStatus:%{public}d", v.GetP2pDeviceStatus());
    WIFI_LOGI("       WpsConfigMethod:%{public}d", v.GetWpsConfigMethod());
    WIFI_LOGI("       DeviceCapabilitys:%{public}d", v.GetDeviceCapabilitys());
    WIFI_LOGI("       GroupCapabilitys:%{public}d", v.GetGroupCapabilitys());
    return;
}

void WifiBroadCastHelper::Show(const WifiP2pLinkedInfo &v)
{
    WIFI_LOGI("    -WifiP2pLinkedInfo:");

    if (v.GetConnectState() == P2pConnectedState::P2P_CONNECTED) {
        WIFI_LOGI("      connectState: Connected");
    } else if (v.GetConnectState() == P2pConnectedState::P2P_DISCONNECTED) {
        WIFI_LOGI("      connectState: Disconnected");
    }
    WIFI_LOGI("         isGroupOwner: %{public}s", v.IsGroupOwner() ? "true" : "false");
    WIFI_LOGI("         groupOwnerAddress: %{private}s", v.GetGroupOwnerAddress().c_str());
    return;
}

void WifiBroadCastHelper::Show(const WifiP2pServiceInfo &v)
{
    WIFI_LOGI("    -WifiP2pServiceInfo:");
    WIFI_LOGI("       ServiceName:%{public}s", v.GetServiceName().c_str());
    WIFI_LOGI("       deviceAddress:%{private}s.", v.GetDeviceAddress().c_str());
    switch (v.GetServicerProtocolType()) {
        case P2pServicerProtocolType::SERVICE_TYPE_ALL: {
            WIFI_LOGI("       protocolType:SERVICE_TYPE_ALL");
            break;
        }
        case P2pServicerProtocolType::SERVICE_TYPE_BONJOUR: {
            WIFI_LOGI("       protocolType:SERVICE_TYPE_BONJOUR");
            break;
        }
        case P2pServicerProtocolType::SERVICE_TYPE_UP_NP: {
            WIFI_LOGI("       protocolType:SERVICE_TYPE_UP_NP");
            break;
        }
        case P2pServicerProtocolType::SERVICE_TYPE_WS_DISCOVERY: {
            WIFI_LOGI("       protocolType:SERVICE_TYPE_WS_DISCOVERY");
            break;
        }
        case P2pServicerProtocolType::SERVICE_TYPE_VENDOR_SPECIFIC: {
            WIFI_LOGI("       protocolType:SERVICE_TYPE_VENDOR_SPECIFIC");
            break;
        }
        default:
            break;
    }
    return;
}

void WifiBroadCastHelper::Show(const std::vector<WifiP2pServiceInfo> &v)
{
    WIFI_LOGI("    -WifiP2pServiceInfo Vector size[%{public}zu]:", v.size());
    for (auto i : v) {
        Show(i);
    }
    return;
}

void WifiBroadCastHelper::Show(const std::vector<WifiP2pDevice> &v)
{
    WIFI_LOGI("    -WifiP2pDevice Vector size[%{public}zu]:", v.size());
    for (auto i : v) {
        Show(i);
    }
    return;
}

void WifiBroadCastHelper::Show(const std::vector<WifiP2pLinkedInfo> &v)
{
    WIFI_LOGI("    -WifiP2pDevice Vector size[%{public}zu]:", v.size());
    for (auto i : v) {
        Show(i);
    }
    return;
}

void WifiBroadCastHelper::Show(const std::vector<std::string> &v)
{
    WIFI_LOGI("    -str Vector size[%{public}zu]:", v.size());
    for (auto i : v) {
        WIFI_LOGI("    -str:%{public}s", i.c_str());
    }
    return;
}

void WifiBroadCastHelper::Show(const std::map<std::string, std::string> &v)
{
    WIFI_LOGI("    -str and str Map size[%{public}zu]:", v.size());
    for (auto i : v) {
        WIFI_LOGI("    -key str:%{public}s, -value str:%{public}s", i.first.c_str(), i.second.c_str());
    }
    return;
}
}  // namespace Wifi
}  // namespace OHOS