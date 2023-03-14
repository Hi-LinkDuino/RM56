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

const static int TEN_SECOND = 10;

class Net_Bindself_Func_Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Net_Bindself_Func_Test::SetUp() {}

void Net_Bindself_Func_Test::TearDown() {}

void Net_Bindself_Func_Test::SetUpTestCase()
{
    LOG("[SetUp] begin");
    TestSetUp();
    SoftBus_Test_Permission::AddPermission(DEF_PKG_NAME);

    int ret = RegisterDeviceStateDefCallback();
    EXPECT_EQ(SOFTBUS_OK, ret) << "call reg node state callback fail";

    LOG("[SetUp] end");
}

void Net_Bindself_Func_Test::TearDownTestCase()
{
    int ret = UnRegisterDeviceStateDefCallback();
    EXPECT_EQ(SOFTBUS_OK, ret) << "call unReg node state callback fail";

    TestTearDown();
    SoftBus_Test_Permission::RemovePermission(DEF_PKG_NAME);
}

static void WaitOnly(int seconds)
{
    if (seconds < TEN_SECOND) {
        sleep(seconds);
    } else {
        while (seconds > 0) {
            sleep(TEN_SECOND);
            seconds -= TEN_SECOND;
            LOG("###wait:%d", seconds);
        }
    }
}

/**
 * @tc.number : SUB_Softbus_Net_Bind_Func_0100
 * @tc.name     : get net id
 * @tc.desc       : Test networking function
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(Net_Bindself_Func_Test,
         SUB_Softbus_Net_Bind_Func_0100,
         TestSize.Level3)
{
    char* netId = NULL;
    int count = GetRemoteDeviceNetId(&netId);
    LOG("GetRemoteDeviceNetId count:%d", count);
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            LOG("GetRemoteDeviceNetId id:%s", *(&netId + i));
        }
        free(netId);
    }
}

/**
 * @tc.number : SUB_Softbus_Net_Bind_Func_0200
 * @tc.name     : discovery device
 * @tc.desc       : Test networking function
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(Net_Bindself_Func_Test,
         SUB_Softbus_Net_Bind_Func_0200,
         TestSize.Level3)
{
    int ret = StartDiscoveryDevice();
    EXPECT_EQ(SOFTBUS_OK, ret) << "Discover fail";
}

/**
 * @tc.number : SUB_Softbus_Net_Bind_Func_0300
 * @tc.name     : check node
 * @tc.desc       : Test networking function
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(Net_Bindself_Func_Test,
         SUB_Softbus_Net_Bind_Func_0300,
         TestSize.Level3)
{
    int ret = CheckRemoteDeviceIsNull(BOOL_FALSE);
    EXPECT_EQ(SOFTBUS_OK, ret) << "check node fail";
}

/**
 * @tc.number : SUB_Softbus_Net_Bind_Func_0400
 * @tc.name     : check node multi
 * @tc.desc       : Test networking function
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(Net_Bindself_Func_Test,
         SUB_Softbus_Net_Bind_Func_0400,
         TestSize.Level3)
{
    int nodeCount = SetRemoteDeviceNetIdToGarray();
    LOG("######node count:%d", nodeCount);
    EXPECT_TRUE(nodeCount >= DEF_REMOTE_DEV_COUNT)
        << "check node fail, count:" << nodeCount;
}

/**
 * @tc.number : SUB_Softbus_Net_Bind_Func_0500
 * @tc.name     : check node and senddata
 * @tc.desc       : Test networking function
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(Net_Bindself_Func_Test,
         SUB_Softbus_Net_Bind_Func_0500,
         TestSize.Level3)
{
    int ret = CheckRemoteDeviceIsNull(BOOL_TRUE);
    EXPECT_EQ(SOFTBUS_OK, ret) << "check node fail";

    ret = CreateSsAndOpenSession4Ctl();
    EXPECT_EQ(SOFTBUS_OK, ret) << "create Ss and openS[ctrl] fail";
    ret = CreateSsAndOpenSession4Data();
    EXPECT_EQ(SOFTBUS_OK, ret) << "create Ss and openS[data] fail";

    int dataLen = 888;
    int sessionId = GetCurrentSessionId4Ctl();
    char* sendData = (char*)malloc(dataLen);
    ASSERT_TRUE(sendData != NULL);
    (void)memset_s(sendData, dataLen, 'C', dataLen);
    ret = SendBytes(sessionId, sendData, dataLen);
    EXPECT_EQ(SOFTBUS_OK, ret) << "SendBytes[ctrl] fail";
    ret = SendMessage(sessionId, sendData, dataLen);
    EXPECT_EQ(SOFTBUS_OK, ret) << "SendMessage[ctrl] fail";

    ret = SendData4Data(DATA_TYPE_BYTE, dataLen);
    EXPECT_EQ(SOFTBUS_OK, ret) << "SendData4Data(byte) fail";
    ret = SendData4Data(DATA_TYPE_MSG, dataLen);
    EXPECT_EQ(SOFTBUS_OK, ret) << "SendData4Data(msg) fail";

    ret = CloseSessionAndRemoveSs4Data();
    EXPECT_EQ(SOFTBUS_OK, ret) << "close and remove[data] fail";
    ret = CloseSessionAndRemoveSs4Ctl();
    EXPECT_EQ(SOFTBUS_OK, ret) << "close and remove[ctrl] fail";
    free(sendData);
}

/**
 * @tc.number : SUB_Softbus_Net_Bind_Func_0600
 * @tc.name     : jion network and leve network
 * @tc.desc       : Test networking function
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(Net_Bindself_Func_Test,
         SUB_Softbus_Net_Bind_Func_0600,
         TestSize.Level2)
{
    int ret;
    int size = 1024;
    ret = LeaveNetWork();
    EXPECT_EQ(SOFTBUS_OK, ret) << "LeaveNetWork fail";
    WaitOnly(10);
    ret = JoinNetwork();
    EXPECT_EQ(SOFTBUS_OK, ret) << "JoinNetwork fail";
    ret = CreateSsAndOpenSession4Data();
    ASSERT_EQ(SOFTBUS_OK, ret) << "create Ss and openS[data] fail";
    ret = SendData4Data(DATA_TYPE_BYTE, size);
    EXPECT_EQ(SOFTBUS_OK, ret) << "SendData4Data(byte,1K) fail";
    ret = CloseSessionAndRemoveSs4Data();
    EXPECT_EQ(SOFTBUS_OK, ret) << "close session and remove Ss fail";
    ret = LeaveNetWork();
    EXPECT_EQ(SOFTBUS_OK, ret) << "LeaveNetWork fail";
    WaitOnly(20);
    ret = JoinNetwork();
    EXPECT_EQ(SOFTBUS_OK, ret) << "JoinNetwork fail";
}