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
#include "ap_state_machine.h"
#include <typeinfo>
#include "ipv4_address.h"
#include "ipv6_address.h"
#include "ap_stations_manager.h"
#include "ap_monitor.h"
#include "wifi_settings.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiApStateMachine");

namespace OHOS {
namespace Wifi {
ApStateMachine::ApStateMachine(ApStationsManager &apStationsManager, ApRootState &apRootState, ApIdleState &apIdleState,
    ApStartedState &apStartedState, ApMonitor &apMonitor)
    : StateMachine("ApStateMachine"),
      m_ApStationsManager(apStationsManager),
      m_ApRootState(apRootState),
      m_ApIdleState(apIdleState),
      m_ApStartedState(apStartedState),
      m_ApMonitor(apMonitor)
{
    Init();
}

ApStateMachine::~ApStateMachine()
{
    StopDhcpServer();
    if (pDhcpNotify.get() != nullptr) {
        pDhcpNotify.reset(nullptr);
    }
    StopHandlerThread();
}

ApStateMachine::DhcpNotify::DhcpNotify(ApStateMachine &apStateMachine) : m_apStateMachine(apStateMachine)
{}

ApStateMachine::DhcpNotify::~DhcpNotify()
{}

void ApStateMachine::DhcpNotify::OnSuccess(int status, const std::string &ifname, DhcpResult &result)
{
    WIFI_LOGI(
        "Dhcp notify Success. status:%d, ifname:%s, result:%s.", status, ifname.c_str(), result.strYourCli.c_str());
}

void ApStateMachine::DhcpNotify::OnFailed(int status, const std::string &ifname, const std::string &reason)
{
    WIFI_LOGW("Dhcp notify Failed. status:%d, ifname:%s, reason:%s.", status, ifname.c_str(), reason.c_str());
}

void ApStateMachine::DhcpNotify::OnSerExitNotify(const std::string &ifname)
{
    WIFI_LOGD("Dhcp exit notify.ifname:%s.", ifname.c_str());
    if (ifname == IN_INTERFACE) {
        m_apStateMachine.SendMessage(static_cast<int>(ApStatemachineEvent::CMD_FAIL));
    }
}

void ApStateMachine::Init()
{
    if (!InitialStateMachine()) {
        WIFI_LOGE("Ap StateMachine Initialize failed.");
        return;
    }
    StatePlus(&m_ApRootState, nullptr);
    StatePlus(&m_ApIdleState, &m_ApRootState);
    StatePlus(&m_ApStartedState, &m_ApRootState);
    SetFirstState(&m_ApIdleState);
    m_iface = "wlan0";
    StartStateMachine();
}

void ApStateMachine::OnApStateChange(ApState state)
{
    if (WifiSettings::GetInstance().SetHotspotState(static_cast<int>(state))) {
        WIFI_LOGE("WifiSetting change state fail.");
    }

    if (m_Callbacks.OnApStateChangedEvent != nullptr &&
        (state == ApState::AP_STATE_IDLE || state == ApState::AP_STATE_STARTED || state == ApState::AP_STATE_STARTING ||
            state == ApState::AP_STATE_CLOSING)) {
        m_Callbacks.OnApStateChangedEvent(state);
    }
    return;
}

ErrCode ApStateMachine::RegisterApServiceCallbacks(const IApServiceCallbacks &callbacks)
{
    m_Callbacks = callbacks;
    return ErrCode::WIFI_OPT_SUCCESS;
}

void ApStateMachine::BroadCastStationChange(const StationInfo &staInfo, ApStatemachineEvent act)
{
    switch (act) {
        case ApStatemachineEvent::CMD_STATION_JOIN:
            if (m_Callbacks.OnHotspotStaJoinEvent) {
                m_Callbacks.OnHotspotStaJoinEvent(staInfo);
            }
            break;
        case ApStatemachineEvent::CMD_STATION_LEAVE:
            if (m_Callbacks.OnHotspotStaLeaveEvent) {
                m_Callbacks.OnHotspotStaLeaveEvent(staInfo);
            }
            break;
        default:
            WIFI_LOGW("error BroadCastStation msg %{public}d.", act);
            break;
    }
}

bool ApStateMachine::StartDhcpServer()
{
    WIFI_LOGI("Enter:StartDhcpServer");
    Ipv4Address ipv4(Ipv4Address::INVALID_INET_ADDRESS);
    Ipv6Address ipv6(Ipv6Address::INVALID_INET6_ADDRESS);
    if (!m_DhcpdInterface.StartDhcpServer(IN_INTERFACE, ipv4, ipv6, true)) {
        WIFI_LOGE("start dhcpd fail.");
        return false;
    }
    if (!m_DhcpdInterface.SetDhcpEventFunc(IN_INTERFACE, pDhcpNotify.get())) {
        WIFI_LOGW("Set dhcp notify failed.");
    }
    WIFI_LOGI("Start dhcp server for AP finished.");
    return true;
}

bool ApStateMachine::StopDhcpServer()
{
    WIFI_LOGI("Enter:StopDhcpServer");
    if (!m_DhcpdInterface.StopDhcpServer(IN_INTERFACE)) {
        WIFI_LOGE("Close dhcpd fail.");
        return false;
    }
    return true;
}

bool ApStateMachine::GetConnectedStationInfo(std::map<std::string, StationInfo> &result)
{
    return m_DhcpdInterface.GetConnectedStationInfo(IN_INTERFACE, result);
}

void ApStateMachine::RegisterEventHandler()
{
    using namespace std::placeholders;
    using type = void (StateMachine::*)(int, int, int, const std::any &);

    auto handler = std::bind(static_cast<type>(&StateMachine::SendMessage), this, _1, _2, _3, _4);

    m_ApMonitor.RegisterHandler(
        m_iface, [=](ApStatemachineEvent msgName, int param1, int param2, const std::any &messageObj) {
            handler(static_cast<int>(msgName), param1, param2, messageObj);
        });

    m_ApStationsManager.RegisterEventHandler(std::bind(&ApStateMachine::BroadCastStationChange, this, _1, _2));
}
}  // namespace Wifi
}  // namespace OHOS