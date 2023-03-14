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
#include <gtest/gtest.h>
#include <memory>
#define private public
#include "mock_ui_service_callback.h"
#include "ui_service_proxy.h"

using namespace testing::ext;
using namespace testing;
namespace OHOS {
namespace Ace {
class UIServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<UIServiceProxy> proxy_ = nullptr;
};

void UIServiceProxyTest::SetUpTestCase(void)
{}
void UIServiceProxyTest::TearDownTestCase(void)
{}
void UIServiceProxyTest::SetUp()
{}
void UIServiceProxyTest::TearDown()
{}

/*
 * Feature: UIServiceProxy.
 * Function: UIServiceProxy::OnPushCallBack is called.
 * SubFunction: NA.
 * FunctionPoints: NA.
 * EnvConditions: NA.
 * CaseDescription: NA.
 */
HWTEST_F(UIServiceProxyTest, UIServiceProxy_OnPushCallBack_001, TestSize.Level1)
{
    // 1.stub define
    sptr<MockUIServiceCallBack> mockUIServiceStub(new MockUIServiceCallBack());

    sptr<UIServiceProxy> proxy(new UIServiceProxy(mockUIServiceStub));

    Want want;
    const std::string name = "name";
    const std::string jsonPath = "jsonPath";
    const std::string data = "data";
    const std::string extraData = "extraData";
    EXPECT_CALL(*mockUIServiceStub, OnPushCallBack(_, _, _, _, _)).Times(1);
    if (proxy != nullptr) {
        proxy->OnPushCallBack(want, name, jsonPath, data, extraData);
    }
}

/*
 * Feature: UIServiceProxy.
 * Function: UIServiceProxy::OnRequestCallBack is called.
 * SubFunction: NA.
 * FunctionPoints: NA.
 * EnvConditions: NA.
 * CaseDescription: NA.
 */
HWTEST_F(UIServiceProxyTest, UIServiceProxy_OnRequestCallBack_001, TestSize.Level1)
{
    // 1.stub define
    sptr<MockUIServiceCallBack> mockUIServiceStub(new MockUIServiceCallBack());

    sptr<UIServiceProxy> proxy(new UIServiceProxy(mockUIServiceStub));

    Want want;
    const std::string name = "name";
    const std::string data  = "data";
    EXPECT_CALL(*mockUIServiceStub, OnRequestCallBack(_, _, _)).Times(1);

    if (proxy != nullptr) {
        proxy->OnRequestCallBack(want, name, data);
    }
}

/*
 * Feature: UIServiceProxy.
 * Function: UIServiceProxy::OnReturnRequest is called.
 * SubFunction: NA.
 * FunctionPoints: NA.
 * EnvConditions: NA.
 * CaseDescription: NA.
 */
HWTEST_F(UIServiceProxyTest, UIServiceProxy_OnReturnRequest_001, TestSize.Level1)
{
    // 1.stub define
    sptr<MockUIServiceCallBack> mockUIServiceStub(new MockUIServiceCallBack());

    sptr<UIServiceProxy> proxy(new UIServiceProxy(mockUIServiceStub));

    Want want;
    const std::string source = "source";
    const std::string data  = "data";
    const std::string extraData  = "extraData";
    EXPECT_CALL(*mockUIServiceStub, OnReturnRequest(_, _, _, _)).Times(1);

    if (proxy != nullptr) {
        proxy->OnReturnRequest(want, source, data, extraData);
    }
}
}  // namespace Ace
}  // namespace OHOS