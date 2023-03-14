/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#include "ap_monitor.h"
#include <unistd.h>
#include <functional>

#include "wifi_idl_define.h"
#include "ap_stations_manager.h"
#include "internal_message.h"
#include "wifi_settings.h"
#include "ap_state_machine.h"
#include "wifi_ap_hal_interface.h"
#include "wifi_logger.h"
#include "dhcpd_interface.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiApMonitor");

namespace OHOS {
namespace Wifi {
ApMonitor::ApMonitor()
{}

ApMonitor::~ApMonitor()
{
    StopMonitor();
}

void ApMonitor::DealStaJoinOrLeave(const StationInfo &info, ApStatemachineEvent event)
{
    std::any anySta = info;
    WIFI_LOGI("StationChangeEvent  event: [%{public}d(join=3,leave=4)] %{public}s . %{private}s . %{private}s.,",
        event,
        info.deviceName.c_str(),
        info.bssid.c_str(),
        info.ipAddr.c_str());
    SendMessage(m_selectIfacName, event, 0, 0, anySta);
}

void ApMonitor::OnStaJoinOrLeave(const WifiApConnectionNofify &cbInfo)
{
    StationInfo info;
    info.bssid = cbInfo.mac;
    info.deviceName = GETTING_INFO;
    info.ipAddr = GETTING_INFO;
    int event = cbInfo.type;
    if (event == WIFI_IDL_CBK_CMD_STA_JOIN) {
        DealStaJoinOrLeave(info, ApStatemachineEvent::CMD_STATION_JOIN);
    }
    if (event == WIFI_IDL_CBK_CMD_STA_LEAVE) {
        DealStaJoinOrLeave(info, ApStatemachineEvent::CMD_STATION_LEAVE);
    }
}

void ApMonitor::OnHotspotStateEvent(int state) const
{
    WIFI_LOGI("update HotspotConfig result is [%{public}d].", state);
    if (state == WIFI_IDL_CBK_CMD_AP_DISABLE) {
        SendMessage(m_selectIfacName, ApStatemachineEvent::CMD_UPDATE_HOTSPOTCONFIG_RESULT, 0, 0, 0);
    } else if (state == WIFI_IDL_CBK_CMD_AP_ENABLE) {
        SendMessage(m_selectIfacName, ApStatemachineEvent::CMD_UPDATE_HOTSPOTCONFIG_RESULT, 1, 0, 0);
    } else {
        WIFI_LOGE("Error: Incorrect status code [%{public}d].", state);
    }
}

void ApMonitor::StartMonitor()
{
    using namespace std::placeholders;
    IWifiApMonitorEventCallback wifiApEventCallback = {
        std::bind(&ApMonitor::OnStaJoinOrLeave, this, _1),
        std::bind(&ApMonitor::OnHotspotStateEvent, this, _1),
    };
    WifiApHalInterface::GetInstance().RegisterApEvent(wifiApEventCallback);

    std::string iface = "wlan0";
    m_selectIfacName = iface;
    m_setMonitorIface.insert(iface);
}

void ApMonitor::SendMessage(
    const std::string &iface, ApStatemachineEvent msgName, int param1, int param2, const std::any &messageObj) const
{
    if (m_setMonitorIface.count(iface) > 0) {
        auto iter = m_mapHandler.find(iface);
        if (iter != m_mapHandler.end()) {
            WIFI_LOGI("Ap Monitor event: iface [%{public}s], eventID [%{public}d].",
                iface.c_str(),
                static_cast<int>(msgName));
            const auto &handler = iter->second;
            handler(msgName, param1, param2, messageObj);
        } else {
            WIFI_LOGE("iface: %{public}s is not register handler.", iface.c_str());
        }
    } else {
        WIFI_LOGW("iface: %{public}s is not monitor.", iface.c_str());
    }
}

void ApMonitor::StopMonitor()
{
    IWifiApMonitorEventCallback wifiApEventCallback = {};
    WifiApHalInterface::GetInstance().RegisterApEvent(wifiApEventCallback);
}

void ApMonitor::RegisterHandler(const std::string &iface, const std::function<HandlerMethod> &handler)
{
    auto iter = m_mapHandler.find(iface);
    if (iter != m_mapHandler.end()) {
        iter->second = handler;
    } else {
        m_mapHandler.emplace(std::make_pair(iface, handler));
    }
}

void ApMonitor::UnregisterHandler(const std::string &iface)
{
    auto iter = m_mapHandler.find(iface);
    if (iter != m_mapHandler.end()) {
        m_mapHandler.erase(iter);
    }
}
}  // namespace Wifi
}  // namespace OHOS