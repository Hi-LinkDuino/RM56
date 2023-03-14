/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "mock_ui_service_stub.h"

#include "ui_mgr_service.h"

namespace OHOS {
namespace Ace {
using namespace testing::ext;
class UIMgrServiceTest : public testing::Test {
public:
    UIMgrServiceTest() = default;
    virtual ~UIMgrServiceTest() = default;

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};
void UIMgrServiceTest::SetUpTestCase(void)
{}
void UIMgrServiceTest::TearDownTestCase(void)
{}
void UIMgrServiceTest::SetUp()
{}
void UIMgrServiceTest::TearDown()
{}

/*
 * Feature: UIMgrService
 * Function: QueryServiceState
 * SubFunction: NA
 * FunctionPoints: UIMgrService QueryServiceState
 * EnvConditions: NA
 * CaseDescription: Verify that the UIMgrService could query service state.
 */
HWTEST_F(UIMgrServiceTest, UIMgrServiceTest_QueryServiceState_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIMgrServiceTest_QueryServiceState_0100 start";
    const UIServiceRunningState testValue = UIServiceRunningState::STATE_NOT_START;
    auto uiMgrService = DelayedSingleton<UIMgrService>::GetInstance();

    EXPECT_EQ(testValue, uiMgrService->QueryServiceState());

    GTEST_LOG_(INFO) << "UIMgrServiceTest_QueryServiceState_0100 end";
}

/*
 * Feature: UIMgrService
 * Function: GetEventHandler
 * SubFunction: NA
 * FunctionPoints: UIMgrService GetEventHandler
 * EnvConditions: NA
 * CaseDescription: Verify that the UIMgrService could get eventHandler.
 */
HWTEST_F(UIMgrServiceTest, UIMgrServiceTest_GetEventHandler_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIMgrServiceTest_GetEventHandler_0100 start";
    auto uiMgrService = DelayedSingleton<UIMgrService>::GetInstance();

    const std::shared_ptr<EventHandler> retVal = uiMgrService->GetEventHandler();

    EXPECT_EQ(nullptr, retVal.get());

    GTEST_LOG_(INFO) << "UIMgrServiceTest_GetEventHandler_0100 end";
}

/*
 * Feature: UIMgrService
 * Function: OnStart
 * SubFunction: NA
 * FunctionPoints: UIMgrService OnStart
 * EnvConditions: NA
 * CaseDescription: Verify that the UIMgrService OnStart is normal.
 */
HWTEST_F(UIMgrServiceTest, UIMgrServiceTest_OnStart_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIMgrServiceTest_OnStart_0100 start";
    const UIServiceRunningState testValue = UIServiceRunningState::STATE_RUNNING;
    auto uiMgrService = DelayedSingleton<UIMgrService>::GetInstance();

    uiMgrService->OnStart();
    EXPECT_EQ(testValue, uiMgrService->QueryServiceState());
    EXPECT_NE(nullptr, uiMgrService->GetEventHandler().get());

    GTEST_LOG_(INFO) << "UIMgrServiceTest_OnStart_0100 end";
}

/*
 * Feature: UIMgrService
 * Function: RegisterCallBack
 * SubFunction: NA
 * FunctionPoints: UIMgrService RegisterCallBack
 * EnvConditions: NA
 * CaseDescription: Verify that the UIMgrService RegisterCallBack is normal.
 */
HWTEST_F(UIMgrServiceTest, UIMgrServiceTest_RegisterCallBack_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIMgrServiceTest_RegisterCallBack_0100 start";
    const int testVal = static_cast<int>(NO_ERROR);
    const sptr<MockUIServiceStub> uiService(new (std::nothrow) MockUIServiceStub());
    auto uiMgrService = DelayedSingleton<UIMgrService>::GetInstance();
    Want want;
    EXPECT_EQ(testVal, uiMgrService->RegisterCallBack(want, uiService));

    testing::Mock::AllowLeak(uiService);
    GTEST_LOG_(INFO) << "UIMgrServiceTest_RegisterCallBack_0100 end";
}

/*
 * Feature: UIMgrService
 * Function: UnregisterCallBack
 * SubFunction: NA
 * FunctionPoints: UIMgrService UnregisterCallBack
 * EnvConditions: NA
 * CaseDescription: Verify that the UIMgrService UnregisterCallBack is normal.
 */
HWTEST_F(UIMgrServiceTest, UIMgrServiceTest_UnregisterCallBack_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIMgrServiceTest_UnregisterCallBack_0100 start";
    const int testVal = static_cast<int>(NO_ERROR);
    auto uiMgrService = DelayedSingleton<UIMgrService>::GetInstance();
    Want want;
    EXPECT_EQ(testVal, uiMgrService->UnregisterCallBack(want));

    GTEST_LOG_(INFO) << "UIMgrServiceTest_UnregisterCallBack_0100 end";
}

/*
 * Feature: UIMgrService
 * Function: Push
 * SubFunction: NA
 * FunctionPoints: UIMgrService Push
 * EnvConditions: NA
 * CaseDescription: Verify that the UIMgrService Push is normal.
 */
HWTEST_F(UIMgrServiceTest, UIMgrServiceTest_Push_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIMgrServiceTest_Push_0100 start";
    const int testVal = static_cast<int>(NO_ERROR);
    auto uiMgrService = DelayedSingleton<UIMgrService>::GetInstance();
    Want want;
    const std::string name = "name";
    const std::string jsonPath = "jsonPath";
    const std::string data = "data";
    const std::string extraData = "extraData";
    EXPECT_EQ(testVal, uiMgrService->Push(want, name, jsonPath, data, extraData));

    GTEST_LOG_(INFO) << "UIMgrServiceTest_Push_0100 end";
}

/*
 * Feature: UIMgrService
 * Function: Request
 * SubFunction: NA
 * FunctionPoints: UIMgrService Request
 * EnvConditions: NA
 * CaseDescription: Verify that the UIMgrService Request is normal.
 */
HWTEST_F(UIMgrServiceTest, UIMgrServiceTest_Request_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIMgrServiceTest_Request_0100 start";
    const int testVal = static_cast<int>(NO_ERROR);
    auto uiMgrService = DelayedSingleton<UIMgrService>::GetInstance();
    Want want;
    const std::string name = "name";
    const std::string data = "data";
    EXPECT_EQ(testVal, uiMgrService->Request(want, name, data));

    GTEST_LOG_(INFO) << "UIMgrServiceTest_Request_0100 end";
}
/*
 * Feature: UIMgrService
 * Function: ReturnRequest
 * SubFunction: NA
 * FunctionPoints: UIMgrService ReturnRequest
 * EnvConditions: NA
 * CaseDescription: Verify that the UIMgrService ReturnRequest is normal.
 */
HWTEST_F(UIMgrServiceTest, UIMgrServiceTest_ReturnRequest_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIMgrServiceTest_ReturnRequest_0100 start";
    const int testVal = static_cast<int>(NO_ERROR);
    auto uiMgrService = DelayedSingleton<UIMgrService>::GetInstance();
    Want want;
    const std::string source = "source";
    const std::string data = "data";
    const std::string extraData = "extraData";
    EXPECT_EQ(testVal, uiMgrService->ReturnRequest(want, source, data, extraData));

    GTEST_LOG_(INFO) << "UIMgrServiceTest_ReturnRequest_0100 end";
}

/*
 * Feature: UIMgrService
 * Function: OnStop
 * SubFunction: NA
 * FunctionPoints: UIMgrService OnStop
 * EnvConditions: NA
 * CaseDescription: Verify that the UIMgrService OnStop is normal.
 */
HWTEST_F(UIMgrServiceTest, UIMgrServiceTest_OnStop_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIMgrServiceTest_OnStop_0100 start";
    const UIServiceRunningState testValue = UIServiceRunningState::STATE_NOT_START;
    auto uiMgrService = DelayedSingleton<UIMgrService>::GetInstance();

    uiMgrService->OnStop();
    EXPECT_EQ(testValue, uiMgrService->QueryServiceState());
    EXPECT_EQ(nullptr, uiMgrService->GetEventHandler().get());

    GTEST_LOG_(INFO) << "UIMgrServiceTest_OnStop_0100 end";
}
}  // namespace Ace
}  // namespace OHOS