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

#include "p2p_group_operating_state.h"
#include "wifi_p2p_hal_interface.h"
#include "p2p_state_machine.h"
#include "wifi_logger.h"
#include "if_config.h"

DEFINE_WIFILOG_P2P_LABEL("P2pGroupOperatingState");

namespace OHOS {
namespace Wifi {
P2pGroupOperatingState::P2pGroupOperatingState(P2pStateMachine &stateMachine, WifiP2pGroupManager &groupMgr,
    WifiP2pDeviceManager &deviceMgr)
    : State("P2pGroupOperatingState"),
      mProcessFunMap(),
      p2pStateMachine(stateMachine),
      groupManager(groupMgr),
      deviceManager(deviceMgr)
{}

void P2pGroupOperatingState::GoInState()
{
    WIFI_LOGI("             GoInState");
    Init();
}

void P2pGroupOperatingState::GoOutState()
{
    WIFI_LOGI("             GoOutState");
}

void P2pGroupOperatingState::Init()
{
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_FORM_GROUP, &P2pGroupOperatingState::ProcessCmdCreateGroup));
    mProcessFunMap.insert(std::make_pair(
        P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_STARTED, &P2pGroupOperatingState::ProcessGroupStartedEvt));
    mProcessFunMap.insert(std::make_pair(
        P2P_STATE_MACHINE_CMD::CREATE_GROUP_TIMED_OUT, &P2pGroupOperatingState::ProcessCreateGroupTimeOut));
    mProcessFunMap.insert(std::make_pair(
        P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_REMOVED, &P2pGroupOperatingState::ProcessGroupRemovedEvt));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE, &P2pGroupOperatingState::ProcessCmdDisable));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_REMOVE_GROUP, &P2pGroupOperatingState::ProcessCmdRemoveGroup));
    mProcessFunMap.insert(
        std::make_pair(P2P_STATE_MACHINE_CMD::CMD_DELETE_GROUP, &P2pGroupOperatingState::ProcessCmdDeleteGroup));
    mProcessFunMap.insert(std::make_pair(P2P_STATE_MACHINE_CMD::CMD_HID2D_CREATE_GROUP,
        &P2pGroupOperatingState::ProcessCmdHid2dCreateGroup));
}

bool P2pGroupOperatingState::ProcessCmdCreateGroup(const InternalMessage &msg) const
{
    WifiErrorNo ret = WIFI_IDL_OPT_FAILED;
    const int minValidNetworkid = 0;
    WifiP2pConfigInternal config;
    msg.GetMessageObj(config);
    int freq = p2pStateMachine.GetAvailableFreqByBand(config.GetGoBand());
    int netId = config.GetNetId();
    if (netId >= minValidNetworkid) {
        /**
         * Restart the group using an existing network ID.
         */
        WIFI_LOGE("Restart the group using an existing network ID.");
        if ((!config.GetPassphrase().empty() && config.GetPassphrase().length() >= MIN_PSK_LEN &&
            config.GetPassphrase().length() <= MAX_PSK_LEN) ||
            config.GetPassphrase().empty()) {
            if (!p2pStateMachine.SetGroupConfig(config, false)) {
                WIFI_LOGW("Some configuration settings failed!");
            }
            ret = WifiP2PHalInterface::GetInstance().GroupAdd(true, netId, freq);
        }
    } else if (netId == PERSISTENT_NET_ID || netId == TEMPORARY_NET_ID) {
        /**
         * Create a new persistence group.
         */
        WIFI_LOGE("Create a new %{public}s group.", (netId == PERSISTENT_NET_ID) ? "persistence" : "temporary");
        if (config.GetPassphrase().empty() && config.GetGroupName().empty()) {
            ret = WifiP2PHalInterface::GetInstance().GroupAdd((netId == PERSISTENT_NET_ID) ? true : false, netId, freq);
        } else if (!config.GetPassphrase().empty() && !config.GetGroupName().empty() &&
                   config.GetPassphrase().length() >= MIN_PSK_LEN && config.GetPassphrase().length() <= MAX_PSK_LEN) {
            if (p2pStateMachine.DealCreateNewGroupWithConfig(config, freq)) {
                ret = WIFI_IDL_OPT_OK;
            }
        }
    } else {
        WIFI_LOGE("Invalid parameter.");
    }
    if (WifiErrorNo::WIFI_IDL_OPT_FAILED == ret) {
        WIFI_LOGE("p2p configure to CreateGroup failed.");
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::CreateGroup, WIFI_OPT_FAILED);
        p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
    } else {
        const int cgTimedOut = 5000;
        WIFI_LOGI("p2p configure to CreateGroup successful.");
        p2pStateMachine.MessageExecutedLater(
            static_cast<int>(P2P_STATE_MACHINE_CMD::CREATE_GROUP_TIMED_OUT), cgTimedOut);
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::CreateGroup, WIFI_OPT_SUCCESS);
    }
    return EXECUTED;
}

bool P2pGroupOperatingState::ProcessGroupStartedEvt(const InternalMessage &msg) const
{
    p2pStateMachine.StopTimer(static_cast<int>(P2P_STATE_MACHINE_CMD::CREATE_GROUP_TIMED_OUT));
    WifiP2pGroupInfo group;
    WifiP2pDevice owner;
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
    } else {
        group.SetNetworkId(TEMPORARY_NET_ID);
        WIFI_LOGI("This is a temporary group.");
    }

    std::string goAddr = group.GetOwner().GetDeviceAddress();
    if (group.IsGroupOwner()) { /* append setting the device name if this is GO */
        WifiP2pDevice thisDevice = deviceManager.GetThisDevice();
        thisDevice.SetP2pDeviceStatus(P2pDeviceStatus::PDS_CONNECTED);
        thisDevice.SetDeviceAddress(goAddr);
        group.SetOwner(thisDevice);
    } else {
        WifiP2pDevice dev = deviceManager.GetDevices(goAddr);
        dev.SetP2pDeviceStatus(P2pDeviceStatus::PDS_CONNECTED);
        if (dev.IsValid()) {
            group.SetOwner(dev);
        }
    }
    group.SetP2pGroupStatus(P2pGroupStatus::GS_STARTED);
    p2pStateMachine.groupManager.SetCurrentGroup(group);

    if (groupManager.GetCurrentGroup().IsGroupOwner()) {
        if (!p2pStateMachine.StartDhcpServer()) {
            WIFI_LOGE("failed to startup Dhcp server.");
            p2pStateMachine.SendMessage(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_REMOVE_GROUP));
        }
    } else {
        p2pStateMachine.StartDhcpClient();
    }
    SharedLinkManager::SetSharedLinkCount(SHARED_LINKE_COUNT_ON_CONNECTED);
    p2pStateMachine.ChangeConnectedStatus(P2pConnectedState::P2P_CONNECTED);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupFormedState);
    return EXECUTED;
}

bool P2pGroupOperatingState::ProcessCreateGroupTimeOut(const InternalMessage &msg) const
{
    WIFI_LOGI("recv event: %{public}d", msg.GetMessageName());
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
    return EXECUTED;
}

bool P2pGroupOperatingState::ProcessGroupRemovedEvt(const InternalMessage &msg) const
{
    WIFI_LOGI("recv group remove event: %{public}d", msg.GetMessageName());
    if (groupManager.GetCurrentGroup().IsPersistent()) {
        groupManager.StashGroups();
        WifiP2pGroupInfo copy = groupManager.GetCurrentGroup();
        copy.SetP2pGroupStatus(P2pGroupStatus::GS_CREATED);
        copy.SetGoIpAddress(std::string(""));
        groupManager.SetCurrentGroup(copy);
        groupManager.StashGroups();
    }
    p2pStateMachine.ChangeConnectedStatus(P2pConnectedState::P2P_DISCONNECTED);
    IpPool::ReleaseIpPool();
    IfConfig::GetInstance().FlushIpAddr(groupManager.GetCurrentGroup().GetInterface(), IpType::IPTYPE_IPV4);
    SharedLinkManager::SetSharedLinkCount(SHARED_LINKE_COUNT_ON_DISCONNECTED);
    if (groupManager.GetCurrentGroup().IsGroupOwner()) {
        if (!p2pStateMachine.StopDhcpServer()) {
            WIFI_LOGW("failed to stop Dhcp server.");
        }
    } else {
        if (p2pStateMachine.pDhcpService != nullptr) {
            p2pStateMachine.pDhcpService->StopDhcpClient(groupManager.GetCurrentGroup().GetInterface(), false);
        } else {
            WIFI_LOGE("pDhcpService is nullptr, cannot stop dhcp client.");
        }
    }
    WifiP2pGroupInfo invalidGroup;
    groupManager.SetCurrentGroup(invalidGroup);
    p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
    return EXECUTED;
}

bool P2pGroupOperatingState::ProcessCmdDisable(const InternalMessage &msg) const
{
    /**
     * Before disabling P2P, you need to remove the group.
     */
    p2pStateMachine.DelayMessage(&msg);
    return ProcessCmdRemoveGroup(msg);
}

bool P2pGroupOperatingState::ProcessCmdRemoveGroup(const InternalMessage &msg) const
{
    /**
     * Removes a current setup group.
     */
    WIFI_LOGI("recv CMD: %{public}d", msg.GetMessageName());
    WifiErrorNo ret = WIFI_IDL_OPT_FAILED;
    WifiP2pGroupInfo group = groupManager.GetCurrentGroup();
    auto dhcpFunc = [=]() {
        if (!groupManager.GetCurrentGroup().IsGroupOwner()) {
            if (p2pStateMachine.pDhcpService != nullptr) {
                p2pStateMachine.pDhcpService->StopDhcpClient(groupManager.GetCurrentGroup().GetInterface(), false);
            } else {
                WIFI_LOGE("pDhcpService is nullptr, cannot stop dhcp client.");
            }
        } else {
            if (!p2pStateMachine.StopDhcpServer()) {
                WIFI_LOGW("failed to stop Dhcp server.");
            }
        }
    };
    if (group.GetP2pGroupStatus() == P2pGroupStatus::GS_STARTED) {
        /**
         * Only started groups can be removed.
         */
        WIFI_LOGI("now remove : %{private}s.", group.GetInterface().c_str());
        ret = WifiP2PHalInterface::GetInstance().GroupRemove(group.GetInterface());
        if (ret) {
            WIFI_LOGE("P2P group removal failed.");
            dhcpFunc();
            WifiP2pGroupInfo invalidGroup;
            groupManager.SetCurrentGroup(invalidGroup);
            p2pStateMachine.ChangeConnectedStatus(P2pConnectedState::P2P_DISCONNECTED);
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::RemoveGroup, WIFI_OPT_FAILED);
        } else {
            WIFI_LOGI("The P2P group is successfully removed.");
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::RemoveGroup, WIFI_OPT_SUCCESS);
        }
    } else {
        WIFI_LOGE("Error:No group can be removed.");
        p2pStateMachine.ChangeConnectedStatus(P2pConnectedState::P2P_DISCONNECTED);
        p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
    }
    return EXECUTED;
}

bool P2pGroupOperatingState::ProcessCmdDeleteGroup(const InternalMessage &msg) const
{
    /**
     * Delete a group from the group list.
     */
    WIFI_LOGI("Delete a group from the group list.");
    WifiErrorNo ret;
    WifiP2pGroupInfo group;
    WifiP2pGroupInfo currentGroup = groupManager.GetCurrentGroup();
    msg.GetMessageObj(group);
    int networkId = group.GetNetworkId();
    /**
     * If the current group is to be deleted, remove the current group first.
     */
    if (currentGroup.GetP2pGroupStatus() == P2pGroupStatus::GS_STARTED) {
        if (group.GetNetworkId() == currentGroup.GetNetworkId() || group.GetNetworkId() == -1) {
            ProcessCmdRemoveGroup(msg);
        } else {
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupFormedState);
        }
    } else {
        p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
    }

    ret = WifiP2PHalInterface::GetInstance().RemoveNetwork(networkId);
    groupManager.RemoveGroup(group);
    if (ret) {
        WIFI_LOGE("P2P group deletion failed.");
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::DeleteGroup, WIFI_OPT_FAILED);
    } else {
        WIFI_LOGI("The P2P group is deleted successfully.");
        p2pStateMachine.UpdatePersistentGroups();
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::DeleteGroup, WIFI_OPT_SUCCESS);
    }
    return EXECUTED;
}

bool P2pGroupOperatingState::ProcessCmdHid2dCreateGroup(const InternalMessage &msg) const
{
    WifiErrorNo ret = WIFI_IDL_OPT_FAILED;
    int freq = 0;
    msg.GetMessageObj(freq);
    WIFI_LOGI("Create a hid2d group, frequency: %{public}d.", freq);
    ret = WifiP2PHalInterface::GetInstance().GroupAdd(true, PERSISTENT_NET_ID, freq);
    if (WifiErrorNo::WIFI_IDL_OPT_FAILED == ret) {
        WIFI_LOGE("p2p configure to CreateGroup failed.");
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::CreateHid2dGroup, WIFI_OPT_FAILED);
        p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
    } else {
        const int cgTimedOut = 5000;
        WIFI_LOGI("p2p configure hid2d group successful.");
        p2pStateMachine.MessageExecutedLater(
            static_cast<int>(P2P_STATE_MACHINE_CMD::CREATE_GROUP_TIMED_OUT), cgTimedOut);
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::CreateHid2dGroup, WIFI_OPT_SUCCESS);
    }
    return EXECUTED;
}

bool P2pGroupOperatingState::ExecuteStateMsg(InternalMessage *msg)
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
