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

#include "invitation_received_state.h"
#include "wifi_p2p_hal_interface.h"
#include "p2p_state_machine.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("InvitationReceivedState");

namespace OHOS {
namespace Wifi {
InvitationReceivedState::InvitationReceivedState(
    P2pStateMachine &stateMachine, WifiP2pGroupManager &groupMgr, WifiP2pDeviceManager &deviceMgr)
    : State("InvitationReceivedState"), p2pStateMachine(stateMachine), groupManager(groupMgr), deviceManager(deviceMgr)
{}

void InvitationReceivedState::GoInState()
{
    WIFI_LOGI("             GoInState");
    p2pStateMachine.NotifyUserInvitationReceivedMessage();
}

void InvitationReceivedState::GoOutState()
{
    WIFI_LOGI("             GoOutState");
}

bool InvitationReceivedState::ExecuteStateMsg(InternalMessage *msg)
{
    switch (static_cast<P2P_STATE_MACHINE_CMD>(msg->GetMessageName())) {
        case P2P_STATE_MACHINE_CMD::INTERNAL_CONN_USER_ACCEPT: {
            WIFI_LOGI("user accept.");
            const WpsInfo &wps = p2pStateMachine.savedP2pConfig.GetWpsInfo();
            if (wps.GetWpsMethod() == WpsMethod::WPS_METHOD_KEYPAD) {
                std::string inputPin;
                if (!msg->GetMessageObj(inputPin)) {
                    WIFI_LOGD("Failed to obtain the pin code.");
                    break;
                }
                WpsInfo wpsPin = wps;
                wpsPin.SetPin(inputPin);
                WIFI_LOGI("INPUT PIN: [%{private}s] ", wpsPin.GetPin().c_str());
                p2pStateMachine.savedP2pConfig.SetWpsInfo(wpsPin);
            }

            if (WifiErrorNo::WIFI_IDL_OPT_OK != WifiP2PHalInterface::GetInstance().P2pStopFind()) {
                WIFI_LOGE("Failed to stop find.");
            }

            if (!p2pStateMachine.ReawakenPersistentGroup(p2pStateMachine.savedP2pConfig)) {
                p2pStateMachine.P2pConnectByShowingPin(p2pStateMachine.savedP2pConfig);
            }

            deviceManager.UpdateDeviceStatus(
                p2pStateMachine.savedP2pConfig.GetDeviceAddress(), P2pDeviceStatus::PDS_INVITED);
            p2pStateMachine.BroadcastP2pPeersChanged();
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pGroupNegotiationState);
            break;
        }
        case P2P_STATE_MACHINE_CMD::PEER_CONNECTION_USER_REJECT: {
            WIFI_LOGI("User rejected invitation");
            p2pStateMachine.SwitchState(&p2pStateMachine.p2pIdleState);
            break;
        }
        default:
            return NOT_EXECUTED;
    }
    return EXECUTED;
}
} // namespace Wifi
} // namespace OHOS
