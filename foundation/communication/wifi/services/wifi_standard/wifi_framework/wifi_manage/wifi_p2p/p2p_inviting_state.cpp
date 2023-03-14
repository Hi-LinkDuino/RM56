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
#include "p2p_inviting_state.h"
#include "p2p_state_machine.h"
#include "wifi_p2p_hal_interface.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("P2pInvitingState");

namespace OHOS {
namespace Wifi {
P2pInvitingState::P2pInvitingState(
    P2pStateMachine &stateMachine, WifiP2pGroupManager &groupMgr, WifiP2pDeviceManager &deviceMgr)
    : State("P2pInvitingState"), p2pStateMachine(stateMachine), groupManager(groupMgr), deviceManager(deviceMgr)
{}
void P2pInvitingState::GoInState()
{
    WIFI_LOGI("             GoInState");
}

void P2pInvitingState::GoOutState()
{
    WIFI_LOGI("             GoOutState");
}

bool P2pInvitingState::ExecuteStateMsg(InternalMessage *msg)
{
    switch (static_cast<P2P_STATE_MACHINE_CMD>(msg->GetMessageName())) {
        case P2P_STATE_MACHINE_CMD::CMD_DEVICE_DISCOVERS: {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::DiscoverDevices, ErrCode::WIFI_OPT_FAILED);
            break;
        }
        case P2P_STATE_MACHINE_CMD::CMD_DISCOVER_SERVICES: {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::DiscoverServices, ErrCode::WIFI_OPT_FAILED);
            break;
        }
        case P2P_STATE_MACHINE_CMD::CMD_START_LISTEN: {
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::StartP2pListen, ErrCode::WIFI_OPT_FAILED);
            break;
        }
        case P2P_STATE_MACHINE_CMD::CMD_CANCEL_CONNECT: {
            WifiP2PHalInterface::GetInstance().CancelConnect();
            p2pStateMachine.DealGroupCreationFailed();
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
            p2pStateMachine.BroadcastActionResult(P2pActionCallback::P2pCancelConnect, ErrCode::WIFI_OPT_SUCCESS);
            break;
        }
        case P2P_STATE_MACHINE_CMD::P2P_EVENT_DEVICE_LOST: {
            /* Do nothing */
            return NOT_EXECUTED;
        }
        default:
            return NOT_EXECUTED;
    }
    return EXECUTED;
}
} // namespace Wifi
} // namespace OHOS
