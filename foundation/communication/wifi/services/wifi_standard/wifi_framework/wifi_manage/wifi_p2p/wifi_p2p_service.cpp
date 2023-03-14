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

#include "wifi_p2p_service.h"
#include "wifi_settings.h"
#include "wifi_errcode.h"
#include "p2p_define.h"
#include "abstract_ui.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("WifiP2pService");

namespace OHOS {
namespace Wifi {
WifiP2pService::WifiP2pService(P2pStateMachine &p2pStateMachine, WifiP2pDeviceManager &setDeviceMgr,
    WifiP2pGroupManager &setGroupMgr, WifiP2pServiceManager &setSvrMgr)
    : p2pStateMachine(p2pStateMachine),
      deviceManager(setDeviceMgr),
      groupManager(setGroupMgr),
      serviceManager(setSvrMgr)
{}

WifiP2pService::~WifiP2pService()
{
    UnRegisterP2pServiceCallbacks();
}

ErrCode WifiP2pService::EnableP2p()
{
    WIFI_LOGI("EnableP2p");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_ENABLE));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::DisableP2p()
{
    WIFI_LOGI("DisableP2p");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::DiscoverDevices()
{
    WIFI_LOGI("DiscoverDevices");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DEVICE_DISCOVERS));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::StopDiscoverDevices()
{
    WIFI_LOGI("StopDiscoverDevices");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_STOP_DEVICE_DISCOVERS));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::DiscoverServices()
{
    WIFI_LOGI("DiscoverServices");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DISCOVER_SERVICES));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::StopDiscoverServices()
{
    WIFI_LOGI("StopDiscoverServices");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_STOP_DISCOVER_SERVICES));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::PutLocalP2pService(const WifiP2pServiceInfo &srvInfo)
{
    WIFI_LOGI("PutLocalP2pService");
    const std::any info = srvInfo;
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_PUT_LOCAL_SERVICE), info);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::DeleteLocalP2pService(const WifiP2pServiceInfo &srvInfo)
{
    WIFI_LOGI("DeleteLocalP2pService");
    const std::any info = srvInfo;
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DEL_LOCAL_SERVICE), info);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::RequestService(const WifiP2pDevice &device, const WifiP2pServiceRequest &request)
{
    WIFI_LOGI("RequestService");
    const std::any info = std::pair<WifiP2pDevice, WifiP2pServiceRequest>(device, request);
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_REQUEST_SERVICE), info);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::StartP2pListen(int period, int interval)
{
    WIFI_LOGI("StartP2pListen");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_START_LISTEN), period, interval);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::StopP2pListen()
{
    WIFI_LOGI("StopP2pListen");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_STOP_LISTEN));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::CreateGroup(const WifiP2pConfig &config)
{
    WIFI_LOGI("CreateGroup");
    WifiP2pConfigInternal configInternal(config);
    WpsInfo wps;
    wps.SetWpsMethod(WpsMethod::WPS_METHOD_PBC);
    configInternal.SetWpsInfo(wps);
    const std::any info = configInternal;
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_FORM_GROUP), info);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::RemoveGroup()
{
    WIFI_LOGI("RemoveGroup");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_REMOVE_GROUP));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::DeleteGroup(const WifiP2pGroupInfo &group)
{
    WIFI_LOGI("DeleteGroup");
    const std::any info = group;
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DELETE_GROUP), info);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::P2pConnect(const WifiP2pConfig &config)
{
    WIFI_LOGI("P2pConnect");
    WifiP2pConfigInternal configInternal(config);
    WpsInfo wps;
    wps.SetWpsMethod(WpsMethod::WPS_METHOD_PBC);
    configInternal.SetWpsInfo(wps);
    p2pStateMachine.SetIsNeedDhcp(true);
    const std::any info = configInternal;
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CONNECT), info);

    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::P2pCancelConnect()
{
    WIFI_LOGI("P2pCancelConnect");
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CANCEL_CONNECT));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::SetP2pDeviceName(const std::string &devName)
{
    WIFI_LOGI("SetP2pDeviceName");
    const std::any info = devName;
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_SET_DEVICE_NAME), info);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::SetP2pWfdInfo(const WifiP2pWfdInfo &wfdInfo)
{
    WIFI_LOGD("enable = %{public}d device info = %{public}d port = %{public}d throughput = %{public}d\n",
        wfdInfo.GetWfdEnabled(), wfdInfo.GetDeviceInfo(), wfdInfo.GetCtrlPort(), wfdInfo.GetMaxThroughput());
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_SET_WFD_INFO), wfdInfo);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::QueryP2pLinkedInfo(WifiP2pLinkedInfo &linkedInfo)
{
    WIFI_LOGI("QueryP2pLinkedInfo");
    linkedInfo = groupManager.GetP2pInfo();
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::GetCurrentGroup(WifiP2pGroupInfo &group)
{
    WIFI_LOGI("GetCurrentGroup");
    WifiP2pLinkedInfo p2pInfo;
    WifiSettings::GetInstance().GetP2pInfo(p2pInfo);
    if (p2pInfo.GetConnectState() == P2pConnectedState::P2P_DISCONNECTED) {
        return ErrCode::WIFI_OPT_FAILED;
    }
    WifiP2pGroupInfo copy = groupManager.GetCurrentGroup();
    group = copy;
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::GetP2pEnableStatus(int &status)
{
    WIFI_LOGI("GetP2pEnableStatus");
    status = WifiSettings::GetInstance().GetP2pState();
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::GetP2pDiscoverStatus(int &status)
{
    WIFI_LOGI("GetP2pDiscoverStatus");
    status = WifiSettings::GetInstance().GetP2pDiscoverState();
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::GetP2pConnectedStatus(int &status)
{
    WIFI_LOGI("GetP2pConnectedStatus");
    WifiP2pLinkedInfo p2pInfo;
    WifiSettings::GetInstance().GetP2pInfo(p2pInfo);
    status = static_cast<int>(p2pInfo.GetConnectState());
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::QueryP2pDevices(std::vector<WifiP2pDevice> &devices)
{
    WIFI_LOGI("QueryP2pDevices");
    deviceManager.GetDevicesList(devices);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::QueryP2pLocalDevice(WifiP2pDevice &device)
{
    LOGI("QueryP2pLocalDevice");
    device = deviceManager.GetThisDevice();
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::QueryP2pGroups(std::vector<WifiP2pGroupInfo> &groups)
{
    WIFI_LOGI("QueryP2pGroups");
    groups = groupManager.GetGroups();
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::QueryP2pServices(std::vector<WifiP2pServiceInfo> &services)
{
    WIFI_LOGI("QueryP2pServices");
    serviceManager.GetDeviceServices(services);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::RegisterP2pServiceCallbacks(const IP2pServiceCallbacks &callbacks)
{
    WIFI_LOGI("RegisterP2pServiceCallbacks");
    p2pStateMachine.RegisterP2pServiceCallbacks(callbacks);
    return ErrCode::WIFI_OPT_SUCCESS;
}

void WifiP2pService::UnRegisterP2pServiceCallbacks()
{
    IP2pServiceCallbacks callbacks = {};
    RegisterP2pServiceCallbacks(callbacks);
}

ErrCode WifiP2pService::Hid2dCreateGroup(const int frequency, FreqType type)
{
    WIFI_LOGI("Create hid2d group");
    const std::any info = frequency;
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_HID2D_CREATE_GROUP), info);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pService::Hid2dConnect(const Hid2dConnectConfig& config)
{
    WIFI_LOGI("Hid2dConnect");

    bool isNeedDhcp = true;
    if (config.GetDhcpMode() == DhcpMode::CONNECT_GO_NODHCP ||
        config.GetDhcpMode() == DhcpMode::CONNECT_AP_NODHCP) {
        isNeedDhcp = false;
    }
    p2pStateMachine.SetIsNeedDhcp(isNeedDhcp);
    const std::any info = config;
    p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_HID2D_CONNECT), info);
    return ErrCode::WIFI_OPT_SUCCESS;
}
}  // namespace Wifi
}  // namespace OHOS
