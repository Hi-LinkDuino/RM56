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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock_p2p_pendant.h"
#include "mock_wifi_p2p_hal_interface.h"
#include "p2p_inviting_state.h"
#include "mock_p2p_monitor.h"
#include "mock_wifi_settings.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class P2pInvitingStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pP2pInvitingState.reset(
            new P2pInvitingState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
    }
    virtual void TearDown()
    {
        pP2pInvitingState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pMockP2pPendant.reset();
    }

public:
    std::unique_ptr<P2pInvitingState> pP2pInvitingState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(P2pInvitingStateTest, GoInState, TestSize.Level1)
{
    pP2pInvitingState->GoInState();
}

HWTEST_F(P2pInvitingStateTest, GoOutState, TestSize.Level1)
{
    pP2pInvitingState->GoOutState();
}

HWTEST_F(P2pInvitingStateTest, ExecuteStateMsg, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DEVICE_DISCOVERS));
    pP2pInvitingState->ExecuteStateMsg(&msg);

    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_DEVICE_LOST));
    pP2pInvitingState->ExecuteStateMsg(&msg);

    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_DEVICE_FOUND));
    pP2pInvitingState->ExecuteStateMsg(&msg);

    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CANCEL_CONNECT));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), CancelConnect()).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), DealGroupCreationFailed()).WillOnce(Return());
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastActionResult(_, _)).WillOnce(Return());
    pP2pInvitingState->ExecuteStateMsg(&msg);
}
}  // namespace Wifi
}  // namespace OHOS