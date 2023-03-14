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
#ifndef OHOS_MOCK_AP_PENDANT_H
#define OHOS_MOCK_AP_PENDANT_H
#include "mock_p2p_monitor.h"
#include "mock_authorizing_negotiation_request_state.h"
#include "mock_group_formed_state.h"
#include "mock_group_negotiation_state.h"
#include "mock_invitation_recelved_state.h"
#include "mock_invitation_request_state.h"
#include "mock_p2p_default_state.h"
#include "mock_p2p_disabled_state.h"
#include "mock_p2p_disabling_state.h"
#include "mock_p2p_enabled_state.h"
#include "mock_p2p_enabling_state.h"
#include "mock_p2p_group_formation_state.h"
#include "mock_p2p_group_join_state.h"
#include "mock_p2p_group_operating_state.h"
#include "mock_p2p_idle_state.h"
#include "mock_p2p_inviting_state.h"
#include "mock_p2p_state_machine.h"
#include "mock_provision_discovery_state.h"

namespace OHOS {
namespace Wifi {
class MockP2pPendant {
public:
    MockP2pPendant()
        : groupManager(),
          deviceManager(),
          svrManager(),
          mockAuthorizingNegotiationRequestState(p2pStateMachine, groupManager, deviceManager),
          mockGroupFormedState(p2pStateMachine, groupManager, deviceManager),
          mockGroupNegotiationState(p2pStateMachine, groupManager, deviceManager),
          mockInvitationReceivedState(p2pStateMachine, groupManager, deviceManager),
          mockInvitationRequestState(p2pStateMachine, groupManager, deviceManager),
          mockP2pDefaultState(p2pStateMachine),
          mockP2pDisabledState(p2pStateMachine, groupManager, deviceManager, serviceManager),
          mockP2pDisablingState(p2pStateMachine, groupManager, deviceManager),
          mockP2pEnabledState(p2pStateMachine, groupManager, deviceManager),
          mockP2pEnablingState(p2pStateMachine, groupManager, deviceManager),
          mockP2pGroupFormationState(p2pStateMachine, groupManager, deviceManager),
          mockP2pGroupJoinState(p2pStateMachine, groupManager, deviceManager),
          mockP2pGroupOperatingState(p2pStateMachine, groupManager, deviceManager),
          mockP2pIdleState(p2pStateMachine, groupManager, deviceManager),
          mockP2pInvitingState(p2pStateMachine, groupManager, deviceManager),
          mockProvisionDiscoveryState(p2pStateMachine, groupManager, deviceManager),
          p2pStateMachine(mockP2pMonitor, groupManager, deviceManager, svrManager,
              mockAuthorizingNegotiationRequestState, mockGroupFormedState, mockGroupNegotiationState,
              mockInvitationReceivedState, mockInvitationRequestState, mockP2pDefaultState,
              mockP2pDisabledState, mockP2pDisablingState, mockP2pEnabledState, mockP2pEnablingState,
              mockP2pGroupFormationState, mockP2pGroupJoinState, mockP2pGroupOperatingState, mockP2pIdleState,
              mockP2pInvitingState, mockProvisionDiscoveryState)
    {
    }
    ~MockP2pPendant()
    {}

public:
    void AddGroup(const WifiP2pGroupInfo &group)
    {
        groupManager.AddGroup(group);
    }
    void AddDevice(const WifiP2pDevice &device)
    {
        deviceManager.AddDevice(device);
    }
    void AddSvr()
    {}
    void SetCurrentGroup(const WifiP2pGroupInfo &group)
    {
        groupManager.SetCurrentGroup(group);
    }
    MockAuthorizingNegotiationRequestState &GetMockAuthorizingNegotiationRequestState()
    {
        return mockAuthorizingNegotiationRequestState;
    }
    MockGroupFormedState &GetMockGroupFormedState()
    {
        return mockGroupFormedState;
    }
    MockGroupNegotiationState &GetMockGroupNegotiationState()
    {
        return mockGroupNegotiationState;
    }
    MockInvitationReceivedState &GetMockInvitationReceivedState()
    {
        return mockInvitationReceivedState;
    }
    MockInvitationRequestState &GetMockInvitationRequestState()
    {
        return mockInvitationRequestState;
    }
    MockP2pDefaultState &GetMockP2pDefaultState()
    {
        return mockP2pDefaultState;
    }
    MockP2pDisabledState &GetMockP2pDisabledState()
    {
        return mockP2pDisabledState;
    }
    MockP2pDisablingState &GetMockP2pDisablingState()
    {
        return mockP2pDisablingState;
    }

    MockP2pEnabledState &GetMockP2pEnabledState()
    {
        return mockP2pEnabledState;
    }
    MockP2pEnablingState &GetMockP2pEnablingState()
    {
        return mockP2pEnablingState;
    }
    MockP2pGroupFormationState &GetMockP2pGroupFormationState()
    {
        return mockP2pGroupFormationState;
    }
    MockP2pGroupJoinState &GetMockP2pGroupJoinState()
    {
        return mockP2pGroupJoinState;
    }
    MockP2pGroupOperatingState &GetMockP2pGroupOperatingState()
    {
        return mockP2pGroupOperatingState;
    }
    MockP2pIdleState &GetMockP2pIdleState()
    {
        return mockP2pIdleState;
    }
    MockP2pInvitingState &GetMockP2pInvitingState()
    {
        return mockP2pInvitingState;
    }
    MockP2pStateMachine &GetP2pStateMachine()
    {
        return p2pStateMachine;
    }
    MockProvisionDiscoveryState &GetMockProvisionDiscoveryState()
    {
        return mockProvisionDiscoveryState;
    }
    MockP2pMonitor &GetMockP2pMonitor()
    {
        return mockP2pMonitor;
    }

private:
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
    WifiP2pServiceManager svrManager;
    WifiP2pServiceManager serviceManager;
    MockAuthorizingNegotiationRequestState mockAuthorizingNegotiationRequestState;
    MockGroupFormedState mockGroupFormedState;
    MockGroupNegotiationState mockGroupNegotiationState;

    MockInvitationReceivedState mockInvitationReceivedState;
    MockInvitationRequestState mockInvitationRequestState;
    MockP2pDefaultState mockP2pDefaultState;

    MockP2pDisabledState mockP2pDisabledState;
    MockP2pDisablingState mockP2pDisablingState;

    MockP2pEnabledState mockP2pEnabledState;
    MockP2pEnablingState mockP2pEnablingState;
    MockP2pGroupFormationState mockP2pGroupFormationState;
    MockP2pGroupJoinState mockP2pGroupJoinState;
    MockP2pGroupOperatingState mockP2pGroupOperatingState;
    MockP2pIdleState mockP2pIdleState;
    MockP2pInvitingState mockP2pInvitingState;
    MockProvisionDiscoveryState mockProvisionDiscoveryState;

    MockP2pMonitor mockP2pMonitor;
    MockP2pStateMachine p2pStateMachine;
};
}  // namespace Wifi
}  // namespace OHOS
#endif
