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

#include "p2p_idle_state.h"
#include "wifi_p2p_hal_interface.h"
#include "p2p_state_machine.h"
#include "wifi_p2p_temp_disc_event.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("P2pIdleState");

namespace OHOS {
namespace Wifi {
P2pIdleState::P2pIdleState(
    P2pStateMachine &stateMachine, WifiP2pGroupManager &groupMgr, WifiP2pDeviceManager &deviceMgr)
    : State("P2pIdleState"), p2pStateMachine(stateMachine), groupManager(groupMgr), deviceManager(deviceMgr)
{}
void P2pIdleState::GoInState()
{
    Init();
    WIFI_LOGI("             GoInState");
    p2pStateMachine.StopTimer(static_cast<int>(P2P_STATE_MACHINE_CMD::EXCEPTION_TIMED_OUT));
}

void P2pIdleState::GoOutState()
{
    WIFI_LOGI("             GoOutState");
    const int exceptionTimeOut = 120000;
    p2pStateMachine.MessageExecutedLater(
        static_cast<int>(P2P_STATE_MACHINE_CMD::EXCEPTION_TIMED_OUT), exceptionTimeOut);
}

void P2pIdleState::Init()
{
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_STOP_DEVICE_DISCOVERS, &P2pIdleState::ProcessCmdStopDiscPeer));
    mProcessFunMap.insert(std::make_pair(P2P_STATE_MACHINE_CMD::CMD_CONNECT, &P2pIdleState::ProcessCmdConnect));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_PBC_REQ, &P2pIdleState::ProcessProvDiscPbcReqEvt));
    mProcessFunMap.insert(std::make_pair(
        P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_ENTER_PIN, &P2pIdleState::ProcessProvDiscEnterPinEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_GO_NEG_REQUEST, &P2pIdleState::ProcessNegotReqEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_SHOW_PIN, &P2pIdleState::ProcessProvDiscShowPinEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_FORM_GROUP, &P2pIdleState::ProcessCmdCreateGroup));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_REMOVE_GROUP, &P2pIdleState::ProcessCmdRemoveGroup));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_DELETE_GROUP, &P2pIdleState::ProcessCmdDeleteGroup));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_STARTED, &P2pIdleState::ProcessGroupStartedEvt));
    mProcessFunMap.insert(std::make_pair(
        P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RECEIVED, &P2pIdleState::ProcessInvitationReceivedEvt));
    mProcessFunMap.insert(std::make_pair(P2P_STATE_MACHINE_CMD::CMD_HID2D_CREATE_GROUP,
        &P2pIdleState::ProcessCmdHid2dCreateGroup));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_HID2D_CONNECT, &P2pIdleState::ProcessCmdHid2dConnect));
}

bool P2pIdleState::ProcessCmdStopDiscPeer(InternalMessage &msg) const
{
    WIFI_LOGI("recv CMD: %{public}d", msg.GetMessageName());
    WifiErrorNo retCode = WifiP2PHalInterface::GetInstance().P2pStopFind();
    if (retCode == WifiErrorNo::WIFI_IDL_OPT_OK) {
        retCode = WifiP2PHalInterface::GetInstance().P2pFlush();
        if (retCode != WifiErrorNo::WIFI_IDL_OPT_OK) {
            WIFI_LOGE("call P2pFlush() failed, ErrCode: %{public}d", static_cast<int>(retCode));
        }
        p2pStateMachine.serviceManager.SetQueryId(std::string(""));
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::StopDiscoverDevices, ErrCode::WIFI_OPT_SUCCESS);
    } else {
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::StopDiscoverDevices, ErrCode::WIFI_OPT_FAILED);
    }
    return EXECUTED;
}

bool P2pIdleState::ProcessCmdConnect(InternalMessage &msg) const
{
    WifiP2pConfigInternal config;
    if (!msg.GetMessageObj(config)) {
        WIFI_LOGD("p2p connect Parameter error.");
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_INVALID_PARAM);
        return EXECUTED;
    }
    p2pStateMachine.savedP2pConfig = config;

    P2pConfigErrCode ret = p2pStateMachine.IsConfigUnusable(p2pStateMachine.savedP2pConfig);
    if (ret != P2pConfigErrCode::SUCCESS) {
        WIFI_LOGD("Invalid device information.");
        if (ret == P2pConfigErrCode::MAC_EMPTY) {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_INVALID_PARAM);
        } else if (ret == P2pConfigErrCode::MAC_NOT_FOUND) {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_P2P_MAC_NOT_FOUND);
        } else if (ret == P2pConfigErrCode::ERR_MAC_FORMAT) {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_P2P_ERR_MAC_FORMAT);
        } else if (ret == P2pConfigErrCode::ERR_INTENT) {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_P2P_ERR_INTENT);
        } else if (ret == P2pConfigErrCode::ERR_SIZE_NW_NAME) {
            p2pStateMachine.BroadcastActionResult(
                P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_P2P_ERR_SIZE_NW_NAME);
        }
        return EXECUTED;
    } else {
        if (WifiErrorNo::WIFI_IDL_OPT_OK != WifiP2PHalInterface::GetInstance().P2pStopFind()) {
            WIFI_LOGE("Attempt to connect but cannot stop find");
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_FAILED);
            return EXECUTED;
        }

        if (p2pStateMachine.ReawakenPersistentGroup(p2pStateMachine.savedP2pConfig)) {
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupNegotiationState);
        } else {
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pProvisionDiscoveryState);
        }

        deviceManager.UpdateDeviceStatus(
            p2pStateMachine.savedP2pConfig.GetDeviceAddress(), P2pDeviceStatus::PDS_INVITED);
        p2pStateMachine.BroadcastP2pPeersChanged();
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_SUCCESS);
    }

    return EXECUTED;
}

bool P2pIdleState::ProcessCmdHid2dConnect(InternalMessage &msg) const
{
    WIFI_LOGI("Idle state hid2d connect recv CMD: %{public}d", msg.GetMessageName());

    Hid2dConnectConfig config;
    if (!msg.GetMessageObj(config)) {
        WIFI_LOGE("Hid2d connect:Failed to obtain config info.");
        return EXECUTED;
    }
    if (WifiErrorNo::WIFI_IDL_OPT_OK !=
        WifiP2PHalInterface::GetInstance().Hid2dConnect(config)) {
        WIFI_LOGE("Hid2d Connection failed.");
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::Hid2dConnect, ErrCode::WIFI_OPT_FAILED);
    }
    return EXECUTED;
}

bool P2pIdleState::ProcessProvDiscPbcReqEvt(InternalMessage &msg) const
{
    WIFI_LOGI("recv CMD: %{public}d", msg.GetMessageName());
    return EXECUTED;
}

bool P2pIdleState::ProcessProvDiscEnterPinEvt(InternalMessage &msg) const
{
    WIFI_LOGI("recv CMD: %{public}d", msg.GetMessageName());
    return EXECUTED;
}

bool P2pIdleState::ProcessNegotReqEvt(InternalMessage &msg) const
{
    WifiP2pConfigInternal conf;
    if (!msg.GetMessageObj(conf)) {
        WIFI_LOGD("Failed to obtain conf.");
        return EXECUTED;
    }
    p2pStateMachine.savedP2pConfig = conf;
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pAuthorizingNegotiationRequestState);
    return EXECUTED;
}

bool P2pIdleState::ProcessProvDiscShowPinEvt(InternalMessage &msg) const
{
    WifiP2pTempDiscEvent provDisc;
    if (!msg.GetMessageObj(provDisc)) {
        WIFI_LOGD("Failed to obtain provDisc.");
        return EXECUTED;
    }
    WifiP2pConfigInternal config;
    WpsInfo wps;
    wps.SetWpsMethod(WpsMethod::WPS_METHOD_KEYPAD);
    wps.SetPin(provDisc.GetPin());
    config.SetWpsInfo(wps);
    config.SetDeviceAddress(provDisc.GetDevice().GetDeviceAddress());
    p2pStateMachine.savedP2pConfig = config;

    p2pStateMachine.NotifyUserProvDiscShowPinRequestMessage(provDisc.GetPin(), config.GetDeviceAddress());
    deviceManager.UpdateDeviceStatus(config.GetDeviceAddress(), P2pDeviceStatus::PDS_INVITED);
    p2pStateMachine.BroadcastP2pPeersChanged();
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pAuthorizingNegotiationRequestState);
    return EXECUTED;
}

bool P2pIdleState::ProcessCmdCreateGroup(InternalMessage &msg) const
{
    p2pStateMachine.DelayMessage(&msg);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupOperatingState);
    return EXECUTED;
}

bool P2pIdleState::ProcessCmdRemoveGroup(InternalMessage &msg) const
{
    p2pStateMachine.DelayMessage(&msg);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupOperatingState);
    return EXECUTED;
}

bool P2pIdleState::ProcessCmdDeleteGroup(InternalMessage &msg) const
{
    p2pStateMachine.DelayMessage(&msg);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupOperatingState);
    return EXECUTED;
}

bool P2pIdleState::ProcessGroupStartedEvt(InternalMessage &msg) const
{
    WifiP2pGroupInfo group;
    msg.GetMessageObj(group);
    WIFI_LOGI("P2P_EVENT_GROUP_STARTED create group interface name : %{private}s, network name : %{private}s, owner "
              "address : %{private}s",
        group.GetInterface().c_str(), group.GetGroupName().c_str(), group.GetOwner().GetDeviceAddress().c_str());
    if (group.IsPersistent()) {
        /**
         * Update groups.
         */
        p2pStateMachine.UpdatePersistentGroups();
        group.SetNetworkId(groupManager.GetGroupNetworkId(group.GetOwner(), group.GetGroupName()));
        WIFI_LOGI("the group network id is %{public}d set id is %{public}d",
            group.GetNetworkId(),
            p2pStateMachine.groupManager.GetGroupNetworkId(group.GetOwner(), group.GetGroupName()));
    }
    group.SetP2pGroupStatus(P2pGroupStatus::GS_STARTED);
    p2pStateMachine.groupManager.SetCurrentGroup(group);
    if (!p2pStateMachine.groupManager.GetCurrentGroup().IsGroupOwner()) {
        p2pStateMachine.StartDhcpClient();

        const WifiP2pDevice &owner = groupManager.GetCurrentGroup().GetOwner();
        WifiP2pDevice device = deviceManager.GetDevices(owner.GetDeviceAddress());
        if (device.IsValid()) {
            device.SetP2pDeviceStatus(owner.GetP2pDeviceStatus());
            WifiP2pGroupInfo copy = groupManager.GetCurrentGroup();
            copy.SetOwner(device);
            groupManager.SetCurrentGroup(copy);

            deviceManager.UpdateDeviceStatus(owner.GetDeviceAddress(), P2pDeviceStatus::PDS_CONNECTED);

            p2pStateMachine.BroadcastP2pPeersChanged();
        } else {
            WIFI_LOGD("fail:No GO device information is found.");
        }
    } else {
        WifiP2pGroupInfo currGrp = p2pStateMachine.groupManager.GetCurrentGroup();
        WifiP2pDevice owner = currGrp.GetOwner();
        owner.SetDeviceName(deviceManager.GetThisDevice().GetDeviceName());
        currGrp.SetOwner(owner);
        p2pStateMachine.groupManager.SetCurrentGroup(currGrp);
        if (!p2pStateMachine.StartDhcpServer()) {
            WIFI_LOGE("Failed to start dhcp server.");
            p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_REMOVE_GROUP));
            return EXECUTED;
        }
    }
    SharedLinkManager::SetSharedLinkCount(SHARED_LINKE_COUNT_ON_CONNECTED);
    p2pStateMachine.ChangeConnectedStatus(P2pConnectedState::P2P_CONNECTED);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupFormedState);
    return EXECUTED;
}

bool P2pIdleState::ProcessInvitationReceivedEvt(InternalMessage &msg) const
{
    WifiP2pGroupInfo group;
    if (!msg.GetMessageObj(group)) {
        WIFI_LOGD("p2p invitation received: Parameter error.");
        return EXECUTED;
    }
    const WifiP2pDevice &owner = group.GetOwner();
    if (!owner.IsValid()) {
        int networkId = group.GetNetworkId();
        if (networkId < 0) {
            WIFI_LOGE("Ignore Invalid Invitation");
            return EXECUTED;
        }
        const std::string &ownerAddress = groupManager.GetGroupOwnerAddr(networkId);
        if (ownerAddress.empty()) {
            WIFI_LOGE("Ignore Invalid Invitation");
            return EXECUTED;
        } else {
            WifiP2pDevice device;
            device.SetDeviceAddress(ownerAddress);
            group.SetOwner(device);
        }
    }

    WifiP2pDevice device;
    if (WifiErrorNo::WIFI_IDL_OPT_OK !=
        WifiP2PHalInterface::GetInstance().GetP2pPeer(owner.GetDeviceAddress(), device)) {
        WIFI_LOGW("Failed to get the peer information.");
    } else {
        deviceManager.UpdateDeviceSupplicantInf(device);
    }

    const WifiP2pDevice peer = deviceManager.GetDevices(owner.GetDeviceAddress());
    if (!peer.IsValid()) {
        WIFI_LOGE("Failed to obtain valid device information.");
        return EXECUTED;
    }
    // update the group capabilitys.
    deviceManager.UpdateDeviceGroupCap(peer.GetDeviceAddress(), peer.GetGroupCapabilitys() | 0x01);

    WpsInfo wps;
    if (peer.WpsPbcSupported()) {
        wps.SetWpsMethod(WpsMethod::WPS_METHOD_PBC);
    } else if (peer.WpsDisplaySupported()) {
        wps.SetWpsMethod(WpsMethod::WPS_METHOD_DISPLAY);
    } else if (peer.WpKeypadSupported()) {
        wps.SetWpsMethod(WpsMethod::WPS_METHOD_KEYPAD);
    }

    WifiP2pConfigInternal config;
    config.SetDeviceAddress(owner.GetDeviceAddress());
    config.SetWpsInfo(wps);
    p2pStateMachine.savedP2pConfig = config;

    p2pStateMachine.SwitchState(&p2pStateMachine.p2pInvitationReceivedState);
    return EXECUTED;
}

bool P2pIdleState::ProcessCmdHid2dCreateGroup(InternalMessage &msg) const
{
    p2pStateMachine.DelayMessage(&msg);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupOperatingState);
    return EXECUTED;
}

bool P2pIdleState::ExecuteStateMsg(InternalMessage *msg)
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
