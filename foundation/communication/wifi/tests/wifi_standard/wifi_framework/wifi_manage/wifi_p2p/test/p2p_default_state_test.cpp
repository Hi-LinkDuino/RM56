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
#include "p2p_default_state.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class P2pDefaultStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pDefaultState.reset(new P2pDefaultState(pMockP2pPendant->GetP2pStateMachine()));
    }
    virtual void TearDown()
    {
        pDefaultState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pMockP2pPendant.reset();
    }

public:
    std::unique_ptr<P2pDefaultState> pDefaultState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(P2pDefaultStateTest, GoInState, TestSize.Level1)
{
    pDefaultState->GoInState();
}

HWTEST_F(P2pDefaultStateTest, GoOutState, TestSize.Level1)
{
    pDefaultState->GoOutState();
}

HWTEST_F(P2pDefaultStateTest, ExecuteStateMsg1, TestSize.Level1)
{
    InternalMessage msg;
    std::string deviceAddress;
    msg.SetMessageObj(deviceAddress);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::REMOVE_SERVICE_REQUEST_RECORD));
    EXPECT_TRUE(pDefaultState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pDefaultStateTest, ExecuteStateMsg2, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CREATE_GROUP_TIMED_OUT));
    EXPECT_FALSE(pDefaultState->ExecuteStateMsg(&msg));
}
}  // namespace Wifi
}  // namespace OHOS