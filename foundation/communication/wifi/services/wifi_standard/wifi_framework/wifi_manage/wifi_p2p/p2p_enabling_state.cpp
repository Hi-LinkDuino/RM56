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
#include "p2p_enabling_state.h"
#include "p2p_state_machine.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("P2pEnablingState");

namespace OHOS {
namespace Wifi {
P2pEnablingState::P2pEnablingState(
    P2pStateMachine &stateMachine, WifiP2pGroupManager &groupMgr, WifiP2pDeviceManager &deviceMgr)
    : State("P2pEnablingState"), p2pStateMachine(stateMachine), groupManager(groupMgr), deviceManager(deviceMgr)
{}
void P2pEnablingState::GoInState()
{
    WIFI_LOGI("             GoInState");
    p2pStateMachine.BroadcastP2pStatusChanged(P2pState::P2P_STATE_STARTING);
}

void P2pEnablingState::GoOutState()
{
    WIFI_LOGI("             GoOutState");
}

bool P2pEnablingState::ExecuteStateMsg(InternalMessage *msg)
{
    switch (static_cast<P2P_STATE_MACHINE_CMD>(msg->GetMessageName())) {
        case P2P_STATE_MACHINE_CMD::WPA_CONNECTED_EVENT: {
            p2pStateMachine.StopTimer(static_cast<int>(P2P_STATE_MACHINE_CMD::ENABLE_P2P_TIMED_OUT));
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
            break;
        }
        case P2P_STATE_MACHINE_CMD::WPA_CONN_FAILED_EVENT:
        case P2P_STATE_MACHINE_CMD::ENABLE_P2P_TIMED_OUT:
        case P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE: {
            p2pStateMachine.p2pMonitor.MonitorEnds(p2pStateMachine.p2pIface);
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pDisabledState);
            p2pStateMachine.BroadcastP2pStatusChanged(P2pState::P2P_STATE_CLOSED);
            break;
        }
        default:
            return NOT_EXECUTED;
    }
    return EXECUTED;
}
} // namespace Wifi
} // namespace OHOS
