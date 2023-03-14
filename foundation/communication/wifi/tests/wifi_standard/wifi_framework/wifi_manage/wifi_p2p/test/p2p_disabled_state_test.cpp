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
#include "p2p_disabled_state.h"
#include "mock_p2p_monitor.h"
#include "mock_wifi_settings.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class P2pDisabledStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pMockMonitor = &pMockP2pPendant->GetMockP2pMonitor();
        pP2pDisabledState.reset(
            new P2pDisabledState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager, serviceManager));
    }
    virtual void TearDown()
    {
        pP2pDisabledState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pMockP2pPendant.reset();
        pMockMonitor = nullptr;
    }

public:
    std::unique_ptr<P2pDisabledState> pP2pDisabledState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    MockP2pMonitor* pMockMonitor;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
    WifiP2pServiceManager serviceManager;
};

HWTEST_F(P2pDisabledStateTest, GoInState, TestSize.Level1)
{
    pP2pDisabledState->GoInState();
}

HWTEST_F(P2pDisabledStateTest, GoOutState, TestSize.Level1)
{
    pP2pDisabledState->GoOutState();
}

HWTEST_F(P2pDisabledStateTest, ProcessCmdP2pEnable1, TestSize.Level1)
{
    InternalMessage msg;
    EXPECT_CALL(*pMockMonitor, RegisterIfaceHandler(_, _)).WillOnce(Return());
    EXPECT_CALL(*pMockMonitor, MonitorBegins(_)).WillOnce(Return());
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), StartP2p()).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));

    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetRandomMacAddr(_)).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_ENABLE));
    EXPECT_TRUE(pP2pDisabledState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pDisabledStateTest, ProcessCmdP2pEnable2, TestSize.Level1)
{
    InternalMessage msg;
    EXPECT_CALL(*pMockMonitor, RegisterIfaceHandler(_, _)).WillOnce(Return());
    EXPECT_CALL(*pMockMonitor, MonitorBegins(_)).WillOnce(Return());
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), StartP2p()).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_ENABLE));
    EXPECT_TRUE(pP2pDisabledState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pDisabledStateTest, ExecuteStateMsgFailed, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE));
    EXPECT_FALSE(pP2pDisabledState->ExecuteStateMsg(&msg));
}
}  // namespace Wifi
}  // namespace OHOS