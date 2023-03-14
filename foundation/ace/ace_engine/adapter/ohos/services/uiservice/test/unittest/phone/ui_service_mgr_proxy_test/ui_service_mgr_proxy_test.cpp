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

#include "mock_ui_service_mgr_stub.h"

#include "ui_service_mgr_proxy.h"

namespace OHOS {
namespace Ace {
using namespace testing::ext;
using ::testing::_;

class UIServiceMgrProxyTest : public testing::Test {
public:
    UIServiceMgrProxyTest() = default;
    virtual ~UIServiceMgrProxyTest() = default;

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

private:
    constexpr int TEST_RETVAL_ONREMOTEREQUEST = 1000;
};
void UIServiceMgrProxyTest::SetUpTestCase(void)
{}
void UIServiceMgrProxyTest::TearDownTestCase(void)
{}
void UIServiceMgrProxyTest::SetUp()
{}
void UIServiceMgrProxyTest::TearDown()
{}

/*
 * Feature: UIServiceStub
 * Function: RegisterCallBack
 * SubFunction: NA
 * FunctionPoints: UIServiceStub RegisterCallBack
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceStub RegisterCallBack is normal.
 */
HWTEST_F(UIServiceMgrProxyTest, UIServiceMgrProxyTest_RegisterCallBack_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_RegisterCallBack_0100 start";
    const int testVal = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    sptr<MockUIServiceMgrStub> mockUIServiceMgrStub(new (std::nothrow) MockUIServiceMgrStub());
    std::shared_ptr<UIServiceMgrProxy> uiServiceManagerProxy =
        std::make_shared<UIServiceMgrProxy>(mockUIServiceMgrStub);
    sptr<Ace::IUIService> uiService(new (std::nothrow) MockUIServiceStub());
    Want want;
    const int retVal = uiServiceManagerProxy->RegisterCallBack(want, uiService);

    EXPECT_EQ(testVal, retVal);

    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_RegisterCallBack_0100 end";
}

/*
 * Feature: UIServiceStub
 * Function: UnregisterCallBack
 * SubFunction: NA
 * FunctionPoints: UIServiceStub UnregisterCallBack
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceStub UnregisterCallBack is normal.
 */
HWTEST_F(UIServiceMgrProxyTest, UIServiceMgrProxyTest_UnregisterCallBack_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_UnregisterCallBack_0100 start";
    const int testVal = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    sptr<MockUIServiceMgrStub> mockUIServiceMgrStub(new (std::nothrow) MockUIServiceMgrStub());
    std::shared_ptr<UIServiceMgrProxy> uiServiceManagerProxy =
        std::make_shared<UIServiceMgrProxy>(mockUIServiceMgrStub);
    Want want;
    const int retVal = uiServiceManagerProxy->UnregisterCallBack(want);

    EXPECT_EQ(testVal, retVal);

    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_UnregisterCallBack_0100 end";
}

/*
 * Feature: UIServiceStub
 * Function: Push
 * SubFunction: NA
 * FunctionPoints: UIServiceStub Push
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceStub Push is normal.
 */
HWTEST_F(UIServiceMgrProxyTest, UIServiceMgrProxyTest_Push_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_Push_0100 start";
    const int testVal = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    sptr<MockUIServiceMgrStub> mockUIServiceMgrStub(new (std::nothrow) MockUIServiceMgrStub());
    std::shared_ptr<UIServiceMgrProxy> uiServiceManagerProxy =
        std::make_shared<UIServiceMgrProxy>(mockUIServiceMgrStub);
    sptr<Ace::IUIService> uiService(new (std::nothrow) MockUIServiceStub());
    Want want;
    const std::string name = "name";
    const std::string jsonPath = "jsonPath";
    const std::string data = "data";
    const std::string extraData = "extraData";
    const int retVal = uiServiceManagerProxy->Push(want, name, jsonPath, data, extraData);
    EXPECT_EQ(testVal, retVal);

    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_Push_0100 end";
}

/*
 * Feature: UIServiceStub
 * Function: Request
 * SubFunction: NA
 * FunctionPoints: UIServiceStub Request
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceStub Request is normal.
 */
HWTEST_F(UIServiceMgrProxyTest, UIServiceMgrProxyTest_Request_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_Request_0100 start";
    const int testVal = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    sptr<MockUIServiceMgrStub> mockUIServiceMgrStub(new (std::nothrow) MockUIServiceMgrStub());
    std::shared_ptr<UIServiceMgrProxy> uiServiceManagerProxy =
        std::make_shared<UIServiceMgrProxy>(mockUIServiceMgrStub);
    sptr<Ace::IUIService> uiService(new (std::nothrow) MockUIServiceStub());
    Want want;
    const std::string name = "name";
    const std::string data = "data";
    const int retVal = uiServiceManagerProxy->Request(want, name, data);

    EXPECT_EQ(testVal, retVal);

    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_Request_0100 end";
}
/*
 * Feature: UIServiceStub
 * Function: Request
 * SubFunction: NA
 * FunctionPoints: UIServiceStub Request
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceStub Request is normal.
 */
HWTEST_F(UIServiceMgrProxyTest, UIServiceMgrProxyTest_ReturnRequest_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_ReturnRequest_0100 start";
    const int testVal = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    sptr<MockUIServiceMgrStub> mockUIServiceMgrStub(new (std::nothrow) MockUIServiceMgrStub());
    std::shared_ptr<UIServiceMgrProxy> uiServiceManagerProxy =
        std::make_shared<UIServiceMgrProxy>(mockUIServiceMgrStub);
    sptr<Ace::IUIService> uiService(new (std::nothrow) MockUIServiceStub());
    Want want;
    const std::string source = "source";
    const std::string data = "data";
    const std::string extraData = "extraData";
    const int retVal = uiServiceManagerProxy->ReturnRequest(want, source, data, extraData);

    EXPECT_EQ(testVal, retVal);

    GTEST_LOG_(INFO) << "UIServiceMgrProxyTest_ReturnRequest_0100 end";
}
}  // namespace Ace
}  // namespace OHOS