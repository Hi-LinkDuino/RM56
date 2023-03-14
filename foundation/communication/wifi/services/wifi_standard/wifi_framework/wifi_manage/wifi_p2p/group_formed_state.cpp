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

#include "group_formed_state.h"
#include "wifi_p2p_hal_interface.h"
#include "p2p_state_machine.h"
#include "wifi_p2p_temp_disc_event.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("GroupFormedState");

namespace OHOS {
namespace Wifi {
GroupFormedState::GroupFormedState(
    P2pStateMachine &stateMachine, WifiP2pGroupManager &groupMgr, WifiP2pDeviceManager &deviceMgr)
    : State("GroupFormedState"), p2pStateMachine(stateMachine), groupManager(groupMgr), deviceManager(deviceMgr)
{
    Init();
}
void GroupFormedState::GoInState()
{
    WIFI_LOGI("             GoInState");
    p2pStateMachine.StopTimer(static_cast<int>(P2P_STATE_MACHINE_CMD::EXCEPTION_TIMED_OUT));
}

void GroupFormedState::GoOutState()
{
    WIFI_LOGI("             GoOutState");
}

void GroupFormedState::Init()
{
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::AP_STA_CONNECTED, &GroupFormedState::ProcessConnectEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::AP_STA_DISCONNECTED, &GroupFormedState::ProcessDisconnectEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_DEVICE_LOST, &GroupFormedState::ProcessDeviceLostEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_REMOVE_GROUP, &GroupFormedState::ProcessCmdRemoveGroup));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_DELETE_GROUP, &GroupFormedState::ProcessCmdDeleteGroup));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_REMOVED, &GroupFormedState::ProcessGroupRemovedEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_CONNECT, &GroupFormedState::ProcessCmdConnect));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_PBC_REQ, &GroupFormedState::ProcessProvDiscEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_ENTER_PIN, &GroupFormedState::ProcessProvDiscEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_SHOW_PIN, &GroupFormedState::ProcessProvDiscEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_STARTED, &GroupFormedState::ProcessGroupStartedEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_DEVICE_DISCOVERS, &GroupFormedState::ProcessCmdDiscoverPeer));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_DISCOVER_SERVICES, &GroupFormedState::ProcessCmdDiscServices));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_START_LISTEN, &GroupFormedState::ProcessCmdStartListen));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE, &GroupFormedState::ProcessCmdDisable));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_CANCEL_CONNECT, &GroupFormedState::ProcessCmdCancelConnect));
}

bool GroupFormedState::ProcessCmdConnect(const InternalMessage &msg) const
{
    WifiP2pConfigInternal config;
    if (!msg.GetMessageObj(config)) {
        WIFI_LOGE("Connect:Failed to obtain config info.");
        return EXECUTED;
    }

    P2pConfigErrCode ret = p2pStateMachine.IsConfigUnusable(config);
    if (ret != P2pConfigErrCode::SUCCESS) {
        WIFI_LOGE("Configuration Unavailable");
        if (ret == P2pConfigErrCode::MAC_EMPTY) {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_INVALID_PARAM);
        } else if (ret == P2pConfigErrCode::MAC_NOT_FOUND) {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_P2P_MAC_NOT_FOUND);
        } else if (ret == P2pConfigErrCode::ERR_MAC_FORMAT) {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_P2P_ERR_MAC_FORMAT);
        }
        return EXECUTED;
    }

    p2pStateMachine.savedP2pConfig = config;
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pInvitationRequestState);
    return EXECUTED;
}

bool GroupFormedState::ProcessProvDiscEvt(const InternalMessage &msg) const
{
    WifiP2pTempDiscEvent procDisc;
    if (!msg.GetMessageObj(procDisc) || !procDisc.GetDevice().IsValid()) {
        WIFI_LOGE("Prov disc :Failed to obtain config info.");
        return EXECUTED;
    }

    p2pStateMachine.savedP2pConfig = WifiP2pConfigInternal();
    p2pStateMachine.savedP2pConfig.SetDeviceAddress(procDisc.GetDevice().GetDeviceAddress());

    WpsInfo wps;
    switch (static_cast<P2P_STATE_MACHINE_CMD>(msg.GetMessageName())) {
        case P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_ENTER_PIN: {
            wps.SetWpsMethod(WpsMethod::WPS_METHOD_KEYPAD);
            break;
        }
        case P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_SHOW_PIN: {
            wps.SetWpsMethod(WpsMethod::WPS_METHOD_DISPLAY);
            wps.SetPin(procDisc.GetPin());
            break;
        }
        default: {
            wps.SetWpsMethod(WpsMethod::WPS_METHOD_PBC);
            break;
        }
    }

    p2pStateMachine.savedP2pConfig.SetWpsInfo(wps);
    if (groupManager.GetCurrentGroup().IsGroupOwner()) {
        p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupJoinState);
    } else {
        /* Do nothing */
    }
    return EXECUTED;
}

bool GroupFormedState::ProcessGroupStartedEvt(const InternalMessage &msg) const
{
    WIFI_LOGI("recv CMD: %{public}d", msg.GetMessageName());
    return EXECUTED;
}

bool GroupFormedState::ProcessCmdDiscoverPeer(const InternalMessage &msg) const
{
    WIFI_LOGI("recv CMD: %{public}d", msg.GetMessageName());
    p2pStateMachine.HandlerDiscoverPeers();
    return EXECUTED;
}

bool GroupFormedState::ProcessGroupRemovedEvt(const InternalMessage &msg) const
{
    /**
     * The group has been removed. The possible cause is that an exception occurs during the connection.
     */
    WIFI_LOGI("The group has been removed.");
    p2pStateMachine.DelayMessage(&msg);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupOperatingState);
    return EXECUTED;
}

bool GroupFormedState::ProcessCmdRemoveGroup(const InternalMessage &msg) const
{
    p2pStateMachine.DelayMessage(&msg);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupOperatingState);
    return EXECUTED;
}

bool GroupFormedState::ProcessCmdDeleteGroup(const InternalMessage &msg) const
{
    p2pStateMachine.DelayMessage(&msg);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupOperatingState);
    return EXECUTED;
}
bool GroupFormedState::ProcessCmdDisable(const InternalMessage &msg) const
{
    p2pStateMachine.DelayMessage(&msg);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupOperatingState);
    return EXECUTED;
}

bool GroupFormedState::ProcessDeviceLostEvt(const InternalMessage &msg) const
{
    WifiP2pDevice device;
    if (!msg.GetMessageObj(device) || !device.IsValid()) {
        WIFI_LOGE("Device lost:Failed to obtain client information.");
        return EXECUTED;
    }

    if (!groupManager.GetCurrentGroup().IsContainsDevice(device)) {
        return NOT_EXECUTED;
    }

    return EXECUTED;
}

bool GroupFormedState::ProcessDisconnectEvt(const InternalMessage &msg) const
{
    WifiP2pDevice device;
    if (!msg.GetMessageObj(device) || !device.IsValid()) {
        WIFI_LOGE("Disconnect:Failed to obtain client information.");
        return EXECUTED;
    }

    IpPool::ReleaseIp(device.GetDeviceAddress());
    device.SetP2pDeviceStatus(P2pDeviceStatus::PDS_AVAILABLE);
    deviceManager.UpdateDeviceStatus(device); // used for peers change event querying device infos
    groupManager.RemoveCurrGroupClient(device);
    p2pStateMachine.BroadcastP2pPeersChanged();
    p2pStateMachine.BroadcastP2pConnectionChanged();
    deviceManager.RemoveDevice(device);
    return EXECUTED;
}

bool GroupFormedState::ProcessConnectEvt(const InternalMessage &msg) const
{
    WifiP2pDevice device;
    if (!msg.GetMessageObj(device) || !device.IsValid()) {
        WIFI_LOGE("Connect:Failed to obtain client information.");
        return EXECUTED;
    }

    if (WifiErrorNo::WIFI_IDL_OPT_OK !=
        WifiP2PHalInterface::GetInstance().SetP2pGroupIdle(groupManager.GetCurrentGroup().GetInterface(), 0)) {
        WIFI_LOGE("fail to set GO Idle time.");
    }
    device.SetP2pDeviceStatus(P2pDeviceStatus::PDS_CONNECTED);
    deviceManager.UpdateDeviceStatus(device);
    WifiP2pDevice memberPeer = deviceManager.GetDevices(device.GetDeviceAddress());
    if (memberPeer.IsValid()) {
        memberPeer.SetP2pDeviceStatus(P2pDeviceStatus::PDS_CONNECTED);
        groupManager.UpdateCurrGroupClient(memberPeer);
    } else {
        groupManager.UpdateCurrGroupClient(device);
    }

    p2pStateMachine.BroadcastP2pPeersChanged();
    p2pStateMachine.BroadcastP2pConnectionChanged();
    return EXECUTED;
}

bool GroupFormedState::ProcessCmdCancelConnect(const InternalMessage &msg) const
{
    WIFI_LOGI("recv CMD: %{public}d", msg.GetMessageName());
    p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pCancelConnect, ErrCode::WIFI_OPT_FAILED);
    return EXECUTED;
}

bool GroupFormedState::ProcessCmdDiscServices(const InternalMessage &msg) const
{
    WIFI_LOGI("recv CMD: %{public}d", msg.GetMessageName());
    WIFI_LOGD("p2p_enabled_state recv CMD_DISCOVER_SERVICES");

    p2pStateMachine.CancelSupplicantSrvDiscReq();
    std::string reqId;
    WifiP2pServiceRequest request;
    WifiP2pDevice device;
    device.SetDeviceAddress(std::string("00:00:00:00:00:00"));
    request.SetProtocolType(P2pServicerProtocolType::SERVICE_TYPE_ALL);
    request.SetTransactionId(p2pStateMachine.serviceManager.GetTransId());

    WifiErrorNo retCode =
        WifiP2PHalInterface::GetInstance().ReqServiceDiscovery(device.GetDeviceAddress(), request.GetTlv(), reqId);
    if (WifiErrorNo::WIFI_IDL_OPT_OK != retCode) {
        WIFI_LOGI("Failed to schedule the P2P service discovery request.");
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::DiscoverServices, ErrCode::WIFI_OPT_FAILED);
        return EXECUTED;
    }
    p2pStateMachine.serviceManager.SetQueryId(reqId);

    retCode = WifiP2PHalInterface::GetInstance().P2pFind(DISC_TIMEOUT_S);
    if (retCode != WifiErrorNo::WIFI_IDL_OPT_OK) {
        WIFI_LOGE("call P2pFind failed, ErrorCode: %{public}d", static_cast<int>(retCode));
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::DiscoverServices, ErrCode::WIFI_OPT_FAILED);
        return EXECUTED;
    }

    WIFI_LOGD("CMD_DISCOVER_SERVICES successful.");
    p2pStateMachine.BroadcastActionResult(P2pActionCallback::DiscoverServices, ErrCode::WIFI_OPT_SUCCESS);
    p2pStateMachine.BroadcastP2pDiscoveryChanged(true);
    return EXECUTED;
}

bool GroupFormedState::ProcessCmdStartListen(const InternalMessage &msg) const
{
    if (WifiP2PHalInterface::GetInstance().P2pFlush()) {
        WIFI_LOGW("Unexpected results in p2p flush.");
    }

    constexpr int defaultOpClass = 81;
    constexpr int defaultChannel = 6;
    if (WifiP2PHalInterface::GetInstance().SetListenChannel(defaultChannel, defaultOpClass)) {
        WIFI_LOGI("p2p set listen channel failed. channel:%{public}d, opclass:%{public}d", defaultChannel,
            defaultOpClass);
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::StartP2pListen, WIFI_OPT_FAILED);
        return EXECUTED;
    }

    size_t period = msg.GetParam1();
    size_t interval = msg.GetParam2();
    if (WifiP2PHalInterface::GetInstance().P2pConfigureListen(true, period, interval)) {
        WIFI_LOGE("p2p configure to start listen failed.");
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::StartP2pListen, WIFI_OPT_FAILED);
    } else {
        WIFI_LOGI("p2p configure to start listen successful.");
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::StartP2pListen, WIFI_OPT_SUCCESS);
    }
    return EXECUTED;
}

bool GroupFormedState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        WIFI_LOGE("fatal error!");
        return NOT_EXECUTED;
    }
    int msgName = msg->GetMessageName();
    auto iter = mProcessFunMap.find(static_cast<P2P_STATE_MACHINE_CMD>(msgName));
    if (iter == mProcessFunMap.end()) {
        return NOT_EXECUTED;
    }
    if ((this->*(iter->second))(*msg)) {
        return EXECUTED;
    } else {
        return NOT_EXECUTED;
    }
}
}  // namespace Wifi
}  // namespace OHOS
