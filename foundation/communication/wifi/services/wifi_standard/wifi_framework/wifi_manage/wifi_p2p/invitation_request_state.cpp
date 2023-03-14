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
#include "invitation_request_state.h"
#include "wifi_p2p_hal_interface.h"
#include "p2p_state_machine.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("InvitationRequestState");

namespace OHOS {
namespace Wifi {
InvitationRequestState::InvitationRequestState(
    P2pStateMachine &stateMachine, WifiP2pGroupManager &groupMgr, WifiP2pDeviceManager &deviceMgr)
    : State("InvitationRequestState"), p2pStateMachine(stateMachine), groupManager(groupMgr), deviceManager(deviceMgr)
{}
void InvitationRequestState::GoInState()
{
    WIFI_LOGI("             GoInState");
    if (WifiErrorNo::WIFI_IDL_OPT_OK == WifiP2PHalInterface::GetInstance().Invite(groupManager.GetCurrentGroup(),
        p2pStateMachine.savedP2pConfig.GetDeviceAddress())) {
        deviceManager.UpdateDeviceStatus(
            p2pStateMachine.savedP2pConfig.GetDeviceAddress(), P2pDeviceStatus::PDS_INVITED);

        p2pStateMachine.BroadcastP2pPeersChanged();
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_SUCCESS);
    } else {
        WIFI_LOGE("Invitation failed.");
        p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pConnect, ErrCode::WIFI_OPT_FAILED);
        p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupFormedState);
    }
}

void InvitationRequestState::GoOutState()
{
    WIFI_LOGI("             GoOutState");
}

bool InvitationRequestState::ExecuteStateMsg(InternalMessage *msg)
{
    switch (static_cast<P2P_STATE_MACHINE_CMD>(msg->GetMessageName())) {
        case P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT: {
            P2pStatus status = static_cast<P2pStatus>(msg->GetParam1());
            WIFI_LOGI("invitation result is %{public}d.", msg->GetParam1());
            if (status == P2pStatus::UNKNOWN_P2P_GROUP) {
                int netId = groupManager.GetCurrentGroup().GetNetworkId();
                if (netId >= 0) {
                    WIFI_LOGD("Remove unknown client from currentGroup");
                    p2pStateMachine.groupManager.RemoveClientFromGroup(
                        netId, p2pStateMachine.savedP2pConfig.GetDeviceAddress());
                    p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupFormedState);
                    p2pStateMachine.SendMessage(
                        static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CONNECT), p2pStateMachine.savedP2pConfig);
                    break;
                }
            }
            if (status == P2pStatus::SUCCESS) {
                WIFI_LOGI("Invitation succeeded.");
            } else {
                WIFI_LOGD("Invitation failed.");
            }
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupFormedState);
            break;
        }
        case P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE: {
            p2pStateMachine.DelayMessage(msg);
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupFormedState);
            break;
        }
        default:
            return NOT_EXECUTED;
    }

    return EXECUTED;
}
} // namespace Wifi
} // namespace OHOS
