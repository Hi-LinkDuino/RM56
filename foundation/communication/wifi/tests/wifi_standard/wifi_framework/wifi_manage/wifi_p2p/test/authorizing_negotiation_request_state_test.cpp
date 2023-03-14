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

#include "authorizing_negotiation_request_state.h"
#include "mock_p2p_pendant.h"
#include "mock_wifi_p2p_hal_interface.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class AuthorizingNegotiationRequestStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pAuthorizingNegotlationRequestState.reset(
            new AuthorizingNegotiationRequestState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
        pGroupFormedState.reset(
            new GroupFormedState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
    }
    virtual void TearDown()
    {
        pAuthorizingNegotlationRequestState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pMockP2pPendant.reset();
        pGroupFormedState.reset();
        pGroupFormedState.reset();
    }

public:
    void AddSaveP2pConfig()
    {
        InternalMessage msg;
        msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_ENTER_PIN));
        WifiP2pTempDiscEvent procDisc;
        WifiP2pDevice device;
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        procDisc.SetDevice(device);
        msg.SetMessageObj(procDisc);
        pGroupFormedState->ExecuteStateMsg(&msg);
    }
    std::unique_ptr<GroupFormedState> pGroupFormedState;
    std::unique_ptr<AuthorizingNegotiationRequestState> pAuthorizingNegotlationRequestState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(AuthorizingNegotiationRequestStateTest, GoInState, TestSize.Level1)
{
    pAuthorizingNegotlationRequestState->GoInState();
    AddSaveP2pConfig();
    pAuthorizingNegotlationRequestState->GoInState();
}

HWTEST_F(AuthorizingNegotiationRequestStateTest, GoOutState, TestSize.Level1)
{
    pAuthorizingNegotlationRequestState->GoOutState();
}

HWTEST_F(AuthorizingNegotiationRequestStateTest, ExecuteStateMsg, TestSize.Level1)
{
    AddSaveP2pConfig();
    InternalMessage msg;
    std::string inputPin;
    msg.SetMessageObj(inputPin);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::INTERNAL_CONN_USER_ACCEPT));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pStopFind()).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pAuthorizingNegotlationRequestState->ExecuteStateMsg(&msg);
}

HWTEST_F(AuthorizingNegotiationRequestStateTest, ExecuteStateMsg1, TestSize.Level1)
{
    AddSaveP2pConfig();
    InternalMessage msg;
    std::string b = "std::any";
    msg.SetMessageObj(b);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::INTERNAL_CONN_USER_ACCEPT));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pStopFind()).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pAuthorizingNegotlationRequestState->ExecuteStateMsg(&msg);
}

HWTEST_F(AuthorizingNegotiationRequestStateTest, ExecuteStateMsg2, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::PEER_CONNECTION_USER_REJECT));
    pAuthorizingNegotlationRequestState->ExecuteStateMsg(&msg);
}

HWTEST_F(AuthorizingNegotiationRequestStateTest, ExecuteStateMsg3, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::INTERNAL_CONN_USER_CONFIRM));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), Connect(_, _, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pAuthorizingNegotlationRequestState->ExecuteStateMsg(&msg);
}

HWTEST_F(AuthorizingNegotiationRequestStateTest, ExecuteStateMsg4, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::REMOVE_SERVICE_REQUEST_RECORD));
    pAuthorizingNegotlationRequestState->ExecuteStateMsg(&msg);
}
}  // namespace Wifi
}  // namespace OHOS