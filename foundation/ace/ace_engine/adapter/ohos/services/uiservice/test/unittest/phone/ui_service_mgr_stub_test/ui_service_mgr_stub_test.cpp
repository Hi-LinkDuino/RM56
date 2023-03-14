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
class UIServiceMgrStubTest : public testing::Test {
public:
    UIServiceMgrStubTest() = default;
    virtual ~UIServiceMgrStubTest() = default;

    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

private:
    constexpr int TEST_RETVAL_ONREMOTEREQUEST = 1000;
};
void UIServiceMgrStubTest::SetUpTestCase(void)
{}
void UIServiceMgrStubTest::TearDownTestCase(void)
{}
void UIServiceMgrStubTest::SetUp()
{}
void UIServiceMgrStubTest::TearDown()
{}

/*
 * Feature: UIServiceMgrStub
 * Function: OnRemoteRequest
 * SubFunction: NA
 * FunctionPoints: UIServiceMgrStub OnRemoteRequest
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceMgrStub OnRemoteRequest is normal.
 */
HWTEST_F(UIServiceMgrStubTest, UIServiceMgrStubTest_OnRemoteRequest_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_OnRemoteRequest_0100 start";
    std::shared_ptr<MockUIServiceMgrStub> uiService = std::make_shared<MockUIServiceMgrStub>();
    const int testVal = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    uint32_t code = IUIServiceMgr::RETURN_REQUEST + 1;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(UIServiceMgrProxy::GetDescriptor())) {
        GTEST_LOG_(ERROR) << "WriteInterfaceToken(data) retval is false end";
        return;
    }

    const int retval = uiService->OnRemoteRequest(code, data, reply, option);

    EXPECT_EQ(testVal, retval);
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_OnRemoteRequest_0100 end";
}

/*
 * Feature: UIServiceMgrStub
 * Function: RegisterCallBack
 * SubFunction: NA
 * FunctionPoints: UIServiceMgrStub RegisterCallBack
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceMgrStub RegisterCallBack is normal.
 */
HWTEST_F(UIServiceMgrStubTest, UIServiceMgrStubTest_RegisterCallBack_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_RegisterCallBack_0100 start";
    const int testVal1 = static_cast<int>(NO_ERROR);
    const int testVal2 = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    std::shared_ptr<MockUIServiceMgrStub> uiService = std::make_shared<MockUIServiceMgrStub>();
    sptr<Ace::IUIService> iUIService(new (std::nothrow) MockUIServiceStub());
    uint32_t code = IUIServiceMgr::REGISTER_CALLBACK;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(UIServiceMgrProxy::GetDescriptor())) {
        GTEST_LOG_(ERROR) << "WriteInterfaceToken(data) retval is false end";
        return;
    }

    Want want;
    data.WriteParcelable(&want);
    if (uiService == nullptr) {
        HILOG_ERROR("register observer fail, uiService is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(iUIService->AsObject())) {
        GTEST_LOG_(ERROR) << "data.WriteRemoteObject(iUIService->AsObject()) retval is false end";
        return;
    }

    EXPECT_CALL(*uiService, RegisterCallBack(testing::_, testing::_)).Times(1).WillOnce(testing::Return(testVal2));

    const int retval1 = uiService->OnRemoteRequest(code, data, reply, option);
    const int retval2 = reply.ReadInt32();

    EXPECT_EQ(testVal1, retval1);
    EXPECT_EQ(testVal2, retval2);
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_RegisterCallBack_0100 end";
}

/*
 * Feature: UIServiceMgrStub
 * Function: UnregisterCallBack
 * SubFunction: NA
 * FunctionPoints: UIServiceMgrStub UnregisterCallBack
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceMgrStub UnregisterCallBack is normal.
 */
HWTEST_F(UIServiceMgrStubTest, UIServiceMgrStubTest_UnregisterCallBack_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_UnregisterCallBack_0100 start";
    const int testVal1 = static_cast<int>(NO_ERROR);
    const int testVal2 = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    std::shared_ptr<MockUIServiceMgrStub> uiService = std::make_shared<MockUIServiceMgrStub>();
    sptr<Ace::IUIService> iUIService(new (std::nothrow) MockUIServiceStub());
    uint32_t code = IUIServiceMgr::UNREGISTER_CALLBACK;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(UIServiceMgrProxy::GetDescriptor())) {
        GTEST_LOG_(ERROR) << "WriteInterfaceToken(data) retval is false end";
        return;
    }
    Want want;
    data.WriteParcelable(&want);
    if (uiService == nullptr) {
        HILOG_ERROR("register observer fail, uiService is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(iUIService->AsObject())) {
        GTEST_LOG_(ERROR) << "data.WriteRemoteObject(iUIService->AsObject()) retval is false end";
        return;
    }

    EXPECT_CALL(*uiService, UnregisterCallBack(testing::_)).Times(1).WillOnce(testing::Return(testVal2));

    const int retval1 = uiService->OnRemoteRequest(code, data, reply, option);
    const int retval2 = reply.ReadInt32();

    EXPECT_EQ(testVal1, retval1);
    EXPECT_EQ(testVal2, retval2);
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_UnregisterCallBack_0100 end";
}
/*
 * Feature: UIServiceMgrStub
 * Function: Push
 * SubFunction: NA
 * FunctionPoints: UIServiceMgrStub Push
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceMgrStub Push is normal.
 */
HWTEST_F(UIServiceMgrStubTest, UIServiceMgrStubTest_Push_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_Push_0100 start";
    const int testVal1 = static_cast<int>(NO_ERROR);
    const int testVal2 = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    std::shared_ptr<MockUIServiceMgrStub> uiService = std::make_shared<MockUIServiceMgrStub>();
    sptr<Ace::IUIService> iUIService(new (std::nothrow) MockUIServiceStub());
    uint32_t code = IUIServiceMgr::PUSH;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(UIServiceMgrProxy::GetDescriptor())) {
        GTEST_LOG_(ERROR) << "WriteInterfaceToken(data) retval is false end";
        return;
    }
    Want want;
    const std::string name = "name";
    const std::string jsonPath = "jsonPath";
    const std::string dataStr = "data";
    const std::string extraData = "extraData";
    data.WriteParcelable(&want);

    if (!data.WriteString(name)) {
        HILOG_ERROR("fail to WriteString name");
        return;
    }

    if (!data.WriteString(jsonPath)) {
        HILOG_ERROR("fail to WriteString jsonPath");
        return;
    }
    if (!data.WriteString(dataStr)) {
        HILOG_ERROR("fail to WriteString dataStr");
        return;
    }
    if (!data.WriteString(extraData)) {
        HILOG_ERROR("fail to WriteString extraData");
        return;
    }
    EXPECT_CALL(*uiService, Push(testing::_, testing::_, testing::_, testing::_,
        testing::_)).Times(1).WillOnce(testing::Return(testVal2));

    const int retval1 = uiService->OnRemoteRequest(code, data, reply, option);
    const int retval2 = reply.ReadInt32();

    EXPECT_EQ(testVal1, retval1);
    EXPECT_EQ(testVal2, retval2);
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_Push_0100 end";
}
/*
 * Feature: UIServiceMgrStub
 * Function: Request
 * SubFunction: NA
 * FunctionPoints: UIServiceMgrStub Request
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceMgrStub Request is normal.
 */
HWTEST_F(UIServiceMgrStubTest, UIServiceMgrStubTest_Request_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_Request_0100 start";
    const int testVal1 = static_cast<int>(NO_ERROR);
    const int testVal2 = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    std::shared_ptr<MockUIServiceMgrStub> uiService = std::make_shared<MockUIServiceMgrStub>();
    sptr<Ace::IUIService> iUIService(new (std::nothrow) MockUIServiceStub());
    uint32_t code = IUIServiceMgr::REQUEST;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(UIServiceMgrProxy::GetDescriptor())) {
        GTEST_LOG_(ERROR) << "WriteInterfaceToken(data) retval is false end";
        return;
    }
    Want want;
    const std::string name = "name";
    const std::string dataStr = "data";
    data.WriteParcelable(&want);

    if (!data.WriteString(name)) {
        HILOG_ERROR("fail to WriteString name");
        return;
    }
    if (!data.WriteString(dataStr)) {
        HILOG_ERROR("fail to WriteString dataStr");
        return;
    }
    EXPECT_CALL(*uiService, Request(testing::_, testing::_,
        testing::_)).Times(1).WillOnce(testing::Return(testVal2));

    const int retval1 = uiService->OnRemoteRequest(code, data, reply, option);
    const int retval2 = reply.ReadInt32();

    EXPECT_EQ(testVal1, retval1);
    EXPECT_EQ(testVal2, retval2);
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_Request_0100 end";
}
/*
 * Feature: UIServiceMgrStub
 * Function: ReturnRequest
 * SubFunction: NA
 * FunctionPoints: UIServiceMgrStub ReturnRequest
 * EnvConditions: NA
 * CaseDescription: Verify that the UIServiceMgrStub ReturnRequest is normal.
 */
HWTEST_F(UIServiceMgrStubTest, UIServiceMgrStubTest_ReturnRequest_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_ReturnRequest_0100 start";
    const int testVal1 = static_cast<int>(NO_ERROR);
    const int testVal2 = static_cast<int>(TEST_RETVAL_ONREMOTEREQUEST);
    std::shared_ptr<MockUIServiceMgrStub> uiService = std::make_shared<MockUIServiceMgrStub>();
    sptr<Ace::IUIService> iUIService(new (std::nothrow) MockUIServiceStub());
    uint32_t code = IUIServiceMgr::RETURN_REQUEST;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(UIServiceMgrProxy::GetDescriptor())) {
        GTEST_LOG_(ERROR) << "WriteInterfaceToken(data) retval is false end";
        return;
    }
    Want want;
    const std::string source = "source";
    const std::string dataStr = "data";
    const std::string extraData = "extraData";
    data.WriteParcelable(&want);

    if (!data.WriteString(source)) {
        HILOG_ERROR("fail to WriteString source");
        return;
    }
    if (!data.WriteString(dataStr)) {
        HILOG_ERROR("fail to WriteString dataStr");
        return;
    }
    if (!data.WriteString(extraData)) {
        HILOG_ERROR("fail to WriteString extraData");
        return;
    }
    EXPECT_CALL(*uiService, ReturnRequest(testing::_, testing::_, testing::_,
        testing::_)).Times(1).WillOnce(testing::Return(testVal2));

    const int retval1 = uiService->OnRemoteRequest(code, data, reply, option);
    const int retval2 = reply.ReadInt32();

    EXPECT_EQ(testVal1, retval1);
    EXPECT_EQ(testVal2, retval2);
    GTEST_LOG_(INFO) << "UIServiceMgrStubTest_ReturnRequest_0100 end";
}
}  // namespace Ace
}  // namespace OHOS