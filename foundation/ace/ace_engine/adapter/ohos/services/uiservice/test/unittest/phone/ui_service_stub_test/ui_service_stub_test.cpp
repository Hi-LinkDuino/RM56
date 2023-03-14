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

#include "mock_ui_service_callback.h"
#define private public
#include "string_ex.h"
#include "ui_service_mgr_interface.h"
#include "ui_service_proxy.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace Ace {
class UIServiceStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void UIServiceStubTest::SetUpTestCase(void)
{}
void UIServiceStubTest::TearDownTestCase(void)
{}
void UIServiceStubTest::SetUp()
{}
void UIServiceStubTest::TearDown()
{}

/*
 * Feature: UIServiceStub.
 * Function: UIServiceStub::UOnRequestCallBack is called.
 * SubFunction: OnRequestCallBack.
 * FunctionPoints: NA.
 * EnvConditions: NA.
 * CaseDescription: NA.
 */
HWTEST_F(UIServiceStubTest, UIServiceStub_remoteDescriptor_001, TestSize.Level1)
{
    sptr<MockUIServiceCallBack> mockUIServiceCallBack(new MockUIServiceCallBack());

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(Str8ToStr16(std::string("descrip_test")));

    EXPECT_CALL(*mockUIServiceCallBack, OnRequestCallBack(_, _, _)).Times(0);
    int res = mockUIServiceCallBack->OnRemoteRequest(
        IUIService::UI_SERVICE_REQUEST_CALL_BACK, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_STATE);
}
/*
 * Feature: UIServiceStub.
 * Function: UIServiceStub::OnPushCallBack is called.
 * SubFunction: OnPushCallBack.
 * FunctionPoints: NA.
 * EnvConditions: NA.
 * CaseDescription: NA.
 */
HWTEST_F(UIServiceStubTest, UIServiceStub_OnPushCallBack_001, TestSize.Level1)
{
    sptr<MockUIServiceCallBack> mockUIServiceCallBack(new MockUIServiceCallBack());

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(UIServiceProxy::GetDescriptor());

    Want want;
    const std::string name = "name";
    const std::string jsonPath = "jsonPath";
    const std::string dataStr = "data";
    const std::string extraData = "extraData";
    data.WriteParcelable(&want);

    if (!data.WriteString(name)) {
        return;
    }

    if (!data.WriteString(jsonPath)) {
        return;
    }
    if (!data.WriteString(dataStr)) {
        return;
    }
    if (!data.WriteString(extraData)) {
        return;
    }
    EXPECT_CALL(*mockUIServiceCallBack, OnPushCallBack(_, _, _, _, _)).Times(1);

    int res = mockUIServiceCallBack->OnRemoteRequest(
        IUIService::UI_SERVICE_PUSH_CALL_BACK, data, reply, option);
    EXPECT_EQ(res, ERR_OK);
}
/*
 * Feature: UIServiceStub.
 * Function: UIServiceStub::OnRequestCallBack is called.
 * SubFunction: OnRequestCallBack.
 * FunctionPoints: NA.
 * EnvConditions: NA.
 * CaseDescription: NA.
 */
HWTEST_F(UIServiceStubTest, UIServiceStub_OnRequestCallBack_001, TestSize.Level1)
{
    sptr<MockUIServiceCallBack> mockUIServiceCallBack(new MockUIServiceCallBack());

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(UIServiceProxy::GetDescriptor());

    Want want;
    const std::string name = "name";
    const std::string dataStr = "data";
    data.WriteParcelable(&want);
    if (!data.WriteString(name)) {
        return;
    }
    if (!data.WriteString(dataStr)) {
        return;
    }
    EXPECT_CALL(*mockUIServiceCallBack, OnRequestCallBack(_,  _, _)).Times(1);

    int res = mockUIServiceCallBack->OnRemoteRequest(
        IUIService::UI_SERVICE_REQUEST_CALL_BACK, data, reply, option);
    EXPECT_EQ(res, ERR_OK);
}

/*
 * Feature: UIServiceStub.
 * Function: UIServiceStub::OnReturnRequest is called.
 * SubFunction: OnReturnRequest.
 * FunctionPoints: NA.
 * EnvConditions: NA.
 * CaseDescription: NA.
 */
HWTEST_F(UIServiceStubTest, UIServiceStub_OnReturnRequest_001, TestSize.Level1)
{
    sptr<MockUIServiceCallBack> mockUIServiceCallBack(new MockUIServiceCallBack());

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(UIServiceProxy::GetDescriptor());

    Want want;
    const std::string source = "source";
    const std::string dataStr = "data";
    const std::string extraData = "extraData";
    data.WriteParcelable(&want);
    if (!data.WriteString(source)) {
        return;
    }
    if (!data.WriteString(dataStr)) {
        return;
    }
    if (!data.WriteString(extraData)) {
        return;
    }
    EXPECT_CALL(*mockUIServiceCallBack, OnReturnRequest(_,  _, _, _)).Times(1);

    int res = mockUIServiceCallBack->OnRemoteRequest(
        IUIService::UI_SERVICE_RETURN_REQUEST, data, reply, option);
    EXPECT_EQ(res, ERR_OK);
}
}  // namespace Ace
}  // namespace OHOS