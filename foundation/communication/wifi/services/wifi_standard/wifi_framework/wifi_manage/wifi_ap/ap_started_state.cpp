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
#include "ap_started_state.h"
#include <cstring>
#include <string>
#include <typeinfo>
#include <vector>
#include <map>
#include "ap_config_use.h"
#include "ap_macro.h"
#include "ap_monitor.h"
#include "ap_service.h"
#include "ap_state_machine.h"
#include "dhcpd_interface.h"
#include "wifi_ap_hal_interface.h"
#include "wifi_ap_nat_manager.h"
#include "ap_stations_manager.h"
#include "wifi_settings.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiApStartedState");
namespace OHOS {
namespace Wifi {
ApStartedState::ApStartedState(ApStateMachine &apStateMachine, ApConfigUse &apConfigUse, ApMonitor &apMonitor)
    : State("ApStartedState"),
      m_hotspotConfig(HotspotConfig()),
      m_ApStateMachine(apStateMachine),
      m_ApConfigUse(apConfigUse),
      m_ApMonitor(apMonitor)
{
    Init();
}

ApStartedState::~ApStartedState()
{}

void ApStartedState::GoInState()
{
    m_ApStateMachine.OnApStateChange(ApState::AP_STATE_STARTING);
    WIFI_LOGI("%{public}s  GoInState", GetStateName().c_str());
    m_ApStateMachine.RegisterEventHandler();
    StartMonitor();

    if (StartAp() == false) {
        WIFI_LOGE("enter ApstartedState is failed.");
        m_ApStateMachine.SwitchState(&m_ApStateMachine.m_ApIdleState);
        return;
    }
    WIFI_LOGI("StartAP is ok.");

    if (SetConfig() == false) {
        WIFI_LOGE("wifi_settings.hotspotconfig is error.");
        m_ApStateMachine.SwitchState(&m_ApStateMachine.m_ApIdleState);
        return;
    }

    if (!m_ApStateMachine.m_ApStationsManager.EnableAllBlockList()) {
        WIFI_LOGE("Set Blocklist failed.");
    }

    WIFI_LOGE("Singleton version has not nat and use wlan0.");
    if (0) {
        if (EnableInterfaceNat() == false) {
            m_ApStateMachine.SwitchState(&m_ApStateMachine.m_ApIdleState);
            return;
        }
    }
    m_ApStateMachine.OnApStateChange(ApState::AP_STATE_STARTED);
}

void ApStartedState::GoOutState()
{
    WIFI_LOGI("%{public}s  GoOutState.", GetStateName().c_str());
    m_ApStateMachine.OnApStateChange(ApState::AP_STATE_CLOSING);
    DisableInterfaceNat();
    m_ApStateMachine.StopDhcpServer();
    if (!StopAp()) {
        WIFI_LOGE("StopAp not going well.");
    }
    StopMonitor();
    m_ApStateMachine.OnApStateChange(ApState::AP_STATE_IDLE);
    WifiSettings::GetInstance().ClearStationList();
}

void ApStartedState::Init()
{
    mProcessFunMap.insert(std::make_pair(ApStatemachineEvent::CMD_FAIL, &ApStartedState::ProcessCmdFail));
    mProcessFunMap.insert(
        std::make_pair(ApStatemachineEvent::CMD_STATION_JOIN, &ApStartedState::ProcessCmdStationJoin));
    mProcessFunMap.insert(
        std::make_pair(ApStatemachineEvent::CMD_STATION_LEAVE, &ApStartedState::ProcessCmdStationLeave));
    mProcessFunMap.insert(std::make_pair(
        ApStatemachineEvent::CMD_SET_HOTSPOT_CONFIG, (ProcessFun)&ApStartedState::ProcessCmdSetHotspotConfig));
    mProcessFunMap.insert(std::make_pair(
        ApStatemachineEvent::CMD_UPDATE_HOTSPOTCONFIG_RESULT, &ApStartedState::ProcessCmdUpdateConfigResult));
    mProcessFunMap.insert(
        std::make_pair(ApStatemachineEvent::CMD_ADD_BLOCK_LIST, &ApStartedState::ProcessCmdAddBlockList));
    mProcessFunMap.insert(
        std::make_pair(ApStatemachineEvent::CMD_DEL_BLOCK_LIST, &ApStartedState::ProcessCmdDelBlockList));
    mProcessFunMap.insert(
        std::make_pair(ApStatemachineEvent::CMD_STOP_HOTSPOT, &ApStartedState::ProcessCmdStopHotspot));
    mProcessFunMap.insert(
        std::make_pair(ApStatemachineEvent::CMD_DISCONNECT_STATION, &ApStartedState::ProcessCmdDisconnectStation));
}

bool ApStartedState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        WIFI_LOGE("fatal error!");
        return false;
    }
    int msgName = msg->GetMessageName();
    WIFI_LOGI("ApStartedState Process msgName:%{public}d.", msgName);

    auto iter = mProcessFunMap.find(static_cast<ApStatemachineEvent>(msgName));
    if (iter == mProcessFunMap.end()) {
        return NOT_EXECUTED;
    }
    ((*this).*(iter->second))(*msg);

    return EXECUTED;
}

bool ApStartedState::SetConfig(HotspotConfig &apConfig)
{
    std::vector<int> allowed5GFreq, allowed2GFreq;
    std::vector<int> allowed5GChan, allowed2GChan;
    if (WifiApHalInterface::GetInstance().GetFrequenciesByBand(static_cast<int>(BandType::BAND_2GHZ), allowed2GFreq)) {
        WIFI_LOGW("failed to get 2.4G channel.");
    }
    if (WifiApHalInterface::GetInstance().GetFrequenciesByBand(static_cast<int>(BandType::BAND_5GHZ), allowed5GFreq)) {
        WIFI_LOGW("failed to get 5G channel.");
    }

    m_ApConfigUse.TransformFrequencyIntoChannel(allowed5GFreq, allowed5GChan);
    m_ApConfigUse.TransformFrequencyIntoChannel(allowed2GFreq, allowed2GChan);

    ChannelsTable channelTbs;
    channelTbs[BandType::BAND_2GHZ] = allowed2GChan;
    channelTbs[BandType::BAND_5GHZ] = allowed5GChan;

    if (WifiSettings::GetInstance().SetValidChannels(channelTbs)) {
        WIFI_LOGE("failed to SetValidChannels.");
        return false;
    }

    m_ApConfigUse.CheckBandChannel(apConfig, channelTbs);

    if (WifiApHalInterface::GetInstance().SetSoftApConfig(apConfig) != WifiErrorNo::WIFI_IDL_OPT_OK) {
        WIFI_LOGE("set hostapd config failed.");
        return false;
    }

    m_ApConfigUse.LogConfig(apConfig);
    WIFI_LOGI("SetConfig OK!.");
    return true;
}

bool ApStartedState::SetConfig()
{
    std::string countryCode;
    if (WifiSettings::GetInstance().GetCountryCode(countryCode)) {
        WIFI_LOGE("get countryCode failed.");
        return false;
    }
    if (WifiApHalInterface::GetInstance().SetWifiCountryCode(countryCode) != WifiErrorNo::WIFI_IDL_OPT_OK) {
        WIFI_LOGE("set countryCode:%{public}s failed.", countryCode.c_str());
        return false;
    }
    WIFI_LOGI("HotspotConfig  CountryCode  = %{public}s.", countryCode.c_str());

    if (WifiSettings::GetInstance().GetHotspotConfig(m_hotspotConfig)) {
        WIFI_LOGE("GetConfig failed!!!.");
        return false;
    }

    return SetConfig(m_hotspotConfig);
}

bool ApStartedState::StartAp() const
{
    WIFI_LOGI("enter startAp.");
    WifiErrorNo retCode = WifiApHalInterface::GetInstance().StartAp();
    if (retCode != WifiErrorNo::WIFI_IDL_OPT_OK) {
        WIFI_LOGE("startAp is failed!");
        return false;
    }
    return true;
}

bool ApStartedState::StopAp() const
{
    WifiErrorNo retCode = WifiApHalInterface::GetInstance().StopAp();
    if (retCode != WifiErrorNo::WIFI_IDL_OPT_OK) {
        return false;
    }
    return true;
}

void ApStartedState::StartMonitor() const
{
    m_ApMonitor.StartMonitor();
}

void ApStartedState::StopMonitor() const
{
    m_ApMonitor.StopMonitor();
}

bool ApStartedState::EnableInterfaceNat() const
{
    if (!mApNatManager.EnableInterfaceNat(true, IN_INTERFACE, OUT_INTERFACE)) {
        WIFI_LOGE("set nat failed.");
        return false;
    }
    return true;
}

bool ApStartedState::DisableInterfaceNat() const
{
    if (!mApNatManager.EnableInterfaceNat(false, IN_INTERFACE, OUT_INTERFACE)) {
        WIFI_LOGE("remove NAT config failed.");
    }
    return true;
}

void ApStartedState::ProcessCmdFail(InternalMessage &msg) const
{
    WIFI_LOGI("State Machine message: %{public}d.", msg.GetMessageName());
    m_ApStateMachine.SwitchState(&m_ApStateMachine.m_ApIdleState);
}

void ApStartedState::ProcessCmdStationJoin(InternalMessage &msg) const
{
    WIFI_LOGI("New station join.");
    StationInfo staInfo;
    if (msg.GetMessageObj(staInfo)) {
        m_ApStateMachine.m_ApStationsManager.StationJoin(staInfo);
    } else {
        WIFI_LOGE("failed to get station info.");
    }
}

void ApStartedState::ProcessCmdStationLeave(InternalMessage &msg) const
{
    WIFI_LOGI("Old station leave.");
    StationInfo staInfo;
    if (msg.GetMessageObj(staInfo)) {
        m_ApStateMachine.m_ApStationsManager.StationLeave(staInfo.bssid);
    } else {
        WIFI_LOGE("failed to get station info.");
    }
}

void ApStartedState::ProcessCmdSetHotspotConfig(InternalMessage &msg)
{
    WIFI_LOGI("Set HotspotConfig.");

    m_hotspotConfig.SetSsid(msg.GetStringFromMessage());
    m_hotspotConfig.SetPreSharedKey(msg.GetStringFromMessage());
    m_hotspotConfig.SetSecurityType(static_cast<KeyMgmt>(msg.GetIntFromMessage()));
    m_hotspotConfig.SetBand(static_cast<BandType>(msg.GetIntFromMessage()));
    m_hotspotConfig.SetChannel(msg.GetIntFromMessage());
    m_hotspotConfig.SetMaxConn(msg.GetIntFromMessage());

    if (SetConfig(m_hotspotConfig)) {
        WIFI_LOGI("SetSoftApConfig success.");
    } else {
        WIFI_LOGE("SetSoftApConfig failed.");
    }
}

void ApStartedState::ProcessCmdUpdateConfigResult(InternalMessage &msg) const
{
    if (msg.GetParam1() == 1) {
        WIFI_LOGI("Hot update HotspotConfig succeeded.");
        if (WifiSettings::GetInstance().SetHotspotConfig(m_hotspotConfig)) {
            WIFI_LOGE("set apConfig to settings failed.");
        }
        m_ApStateMachine.StopDhcpServer();
        m_ApStateMachine.StartDhcpServer();
    } else {
        m_ApStateMachine.SwitchState(&m_ApStateMachine.m_ApIdleState);
    }
}

void ApStartedState::ProcessCmdAddBlockList(InternalMessage &msg) const
{
    WIFI_LOGI("Add block list.");
    StationInfo staInfo;
    staInfo.deviceName = msg.GetStringFromMessage();
    staInfo.bssid = msg.GetStringFromMessage();
    staInfo.ipAddr = msg.GetStringFromMessage();
    WIFI_LOGI("staInfo:%{private}s, %{private}s, %{private}s.", staInfo.deviceName.c_str(), staInfo.bssid.c_str(),
        staInfo.ipAddr.c_str());
    m_ApStateMachine.m_ApStationsManager.AddBlockList(staInfo);
}

void ApStartedState::ProcessCmdDelBlockList(InternalMessage &msg) const
{
    WIFI_LOGI("Delete block list.");
    StationInfo staInfo;
    staInfo.deviceName = msg.GetStringFromMessage();
    staInfo.bssid = msg.GetStringFromMessage();
    staInfo.ipAddr = msg.GetStringFromMessage();
    WIFI_LOGI("staInfo:%{private}s, %{private}s, %{private}s.", staInfo.deviceName.c_str(), staInfo.bssid.c_str(),
        staInfo.ipAddr.c_str());
    m_ApStateMachine.m_ApStationsManager.DelBlockList(staInfo);
}

void ApStartedState::ProcessCmdStopHotspot(InternalMessage &msg) const
{
    WIFI_LOGI("Disable hotspot: %{public}d.", msg.GetMessageName());
    m_ApStateMachine.SwitchState(&m_ApStateMachine.m_ApIdleState);
}

void ApStartedState::ProcessCmdDisconnectStation(InternalMessage &msg) const
{
    WIFI_LOGI("Disconnect station.");
    StationInfo staInfo;
    staInfo.deviceName = msg.GetStringFromMessage();
    staInfo.bssid = msg.GetStringFromMessage();
    staInfo.ipAddr = msg.GetStringFromMessage();
    m_ApStateMachine.m_ApStationsManager.DisConnectStation(staInfo);
}
}  // namespace Wifi
}  // namespace OHOS