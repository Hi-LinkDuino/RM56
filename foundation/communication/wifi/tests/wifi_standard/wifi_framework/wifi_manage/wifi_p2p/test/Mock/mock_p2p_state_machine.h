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
#ifndef OHOS_MOCK_P2P_STATE_MACHINE_H
#define OHOS_MOCK_P2P_STATE_MACHINE_H
#include <gmock/gmock.h>
#include "p2p_state_machine.h"

namespace OHOS {
namespace Wifi {
class MockP2pStateMachine : public P2pStateMachine {
public:
    MockP2pStateMachine(P2pMonitor &p2pMonitor, WifiP2pGroupManager &groupManager, WifiP2pDeviceManager &deviceManager,
        WifiP2pServiceManager &svrManager, AuthorizingNegotiationRequestState &authorizingNegotlationRequestState,
        GroupFormedState &groupFormedState, GroupNegotiationState &groupNegotiationState,
        InvitationReceivedState &invltationRecelvedState, InvitationRequestState &invitationRequestState,
        P2pDefaultState &defaultState, P2pDisabledState &disabledState, P2pDisablingState &disablingState,
        P2pEnabledState &enabledState, P2pEnablingState &enablingState, P2pGroupFormationState &groupFormationState,
        P2pGroupJoinState &groupJoinState, P2pGroupOperatingState &groupOperatingState, P2pIdleState &idleState,
        P2pInvitingState &invitingState, ProvisionDiscoveryState &provisionDiscoveryState)
        : P2pStateMachine(p2pMonitor, groupManager, deviceManager, svrManager, authorizingNegotlationRequestState,
              groupFormedState, groupNegotiationState, invltationRecelvedState, invitationRequestState, defaultState,
              disabledState, disablingState, enabledState, enablingState, groupFormationState, groupJoinState,
              groupOperatingState, idleState, invitingState, provisionDiscoveryState)
    {}
    ~MockP2pStateMachine() = default;
    MOCK_METHOD0(Initialize, void());
    MOCK_METHOD1(IsConfigUnusable, P2pConfigErrCode(const WifiP2pConfigInternal &config));
    MOCK_METHOD0(DealGroupCreationFailed, void());
    MOCK_CONST_METHOD2(BroadcastActionResult, void(P2pActionCallback action, ErrCode result));
    MOCK_CONST_METHOD1(BroadcastP2pStatusChanged, void(P2pState state));
    MOCK_CONST_METHOD1(BroadcastThisDeviceChanaged, void(const WifiP2pDevice &device));
    MOCK_CONST_METHOD0(UpdateGroupInfoToWpa, void());
    MOCK_CONST_METHOD0(BroadcastP2pPeersChanged, void());
    MOCK_CONST_METHOD1(ReawakenPersistentGroup, bool(WifiP2pConfigInternal &config));
    MOCK_CONST_METHOD0(UpdatePersistentGroups, void());
    MOCK_METHOD0(StartDhcpServer, bool());
    MOCK_CONST_METHOD2(SetGroupConfig, bool(const WifiP2pConfigInternal &config, bool newGroup));
    MOCK_CONST_METHOD2(DealCreateNewGroupWithConfig, bool(const WifiP2pConfigInternal &config, int freq));
};
}  // namespace Wifi
}  // namespace OHOS
#endif