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
#include "net_trans_common.h"
#include "SoftBus_Test_Permission.h"
#include "wifi_utils.h"

using namespace testing::ext;

class TransFuncTest : public testing::Test {
public:
    // 测试套前置和后置操作
    static void SetUpTestCase();
    static void TearDownTestCase();

    // 测试用例前置和后置操作
    void SetUp();
    void TearDown();
};

void TransFuncTest::SetUp() {}

void TransFuncTest::TearDown() {}

void TransFuncTest::SetUpTestCase()
{
    LOG("SetUp begin");
    TestSetUp();
    SoftBus_Test_Permission::AddPermission(DEF_PKG_NAME);

    int ret = RegisterDeviceStateDefCallback();
    EXPECT_EQ(SOFTBUS_OK, ret) << "call reg node state callback fail";
    ret = WiFiUtils::CheckIsConnectToDefault();
    ASSERT_EQ(SOFTBUS_OK, ret) << "connect to default failed";
    ret = CheckRemoteDeviceIsNull(BOOL_TRUE);
    ASSERT_EQ(SOFTBUS_OK, ret) << "get node fail,please check network";

    LOG("SetUp end");
}

void TransFuncTest::TearDownTestCase()
{
    int ret = UnRegisterDeviceStateDefCallback();
    EXPECT_EQ(SOFTBUS_OK, ret) << "call unReg node state callback fail";

    TestTearDown();
    SoftBus_Test_Permission::RemovePermission(DEF_PKG_NAME);
}

/**
 * @tc.number : SUB_Softbus_Trans_SendByte_Func_0100
 * @tc.name     : SendByte Packet size 1B, send and receive successful
 * @tc.desc       : Test the SendByte specification
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(TransFuncTest, SUB_Softbus_Trans_SendByte_Func_0100, TestSize.Level3)
{
    int ret;
    ret = CreateSsAndOpenSession4Data();
    ASSERT_EQ(SOFTBUS_OK, ret) << "create Ss and openS[data] fail";

    int size = 1;
    ret = SendData4Data(DATA_TYPE_BYTE, size);
    EXPECT_EQ(SOFTBUS_OK, ret) << "SendData4Data(byte, 1B) fail";

    ret = CloseSessionAndRemoveSs4Data();
    EXPECT_EQ(SOFTBUS_OK, ret) << "close session and remove Ss fail";
}

/**
 * @tc.number : SUB_Softbus_Trans_SendMessage_Func_0100
 * @tc.name     : SendMessage Packet size 1B, send and receive successful
 * @tc.desc       : Test the SendMessage specification
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(TransFuncTest,
         SUB_Softbus_Trans_SendMessage_Func_0100,
         TestSize.Level3)
{
    int ret;
    ret = CreateSsAndOpenSession4Data();
    ASSERT_EQ(SOFTBUS_OK, ret) << "create Ss and openS[data] fail";

    int size = 1;
    ret = SendData4Data(DATA_TYPE_MSG, size);
    EXPECT_EQ(SOFTBUS_OK, ret) << "SendData4Data(msg, 1B) fail";

    ret = CloseSessionAndRemoveSs4Data();
    EXPECT_EQ(SOFTBUS_OK, ret) << "close session and remove Ss fail";
}
