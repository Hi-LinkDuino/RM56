/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include "ap_define.h"
#include "mock_ap_state_machine.h"
#include "ap_root_state.h"

using namespace OHOS;
using ::testing::_;
using ::testing::An;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::TypedEq;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class ApRootState_test : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        pApRootState = new ApRootState();
    }
    virtual void TearDown()
    {
        delete pApRootState;
        pApRootState = nullptr;
    }

public:
    ApRootState *pApRootState;
};

HWTEST_F(ApRootState_test, ExecuteStateMsg_SUCCESS, TestSize.Level1)
{
    InternalMessage *msg = new InternalMessage();

    msg->SetMessageName(static_cast<int>(ApStatemachineEvent::CMD_UPDATE_HOTSPOTCONFIG_RESULT));
    EXPECT_TRUE(pApRootState->ExecuteStateMsg(msg));

    msg->SetMessageName(static_cast<int>(ApStatemachineEvent::CMD_START_HOTSPOT));
    EXPECT_TRUE(pApRootState->ExecuteStateMsg(msg));

    msg->SetMessageName(static_cast<int>(ApStatemachineEvent::CMD_SET_HOTSPOT_CONFIG));
    EXPECT_TRUE(pApRootState->ExecuteStateMsg(msg));
    delete msg;
    msg = nullptr;
}

HWTEST_F(ApRootState_test, ExecuteStateMsg_FAILED, TestSize.Level1)
{
    EXPECT_FALSE(pApRootState->ExecuteStateMsg(nullptr));
}

HWTEST_F(ApRootState_test, GoInState, TestSize.Level1)
{
    pApRootState->GoInState();
}
HWTEST_F(ApRootState_test, GoOutState, TestSize.Level1)
{
    pApRootState->GoOutState();
}
} // namespace Wifi
} // namespace OHOS