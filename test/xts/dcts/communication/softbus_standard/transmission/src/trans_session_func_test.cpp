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

class TransSessionFuncTest : public testing::Test {
public:
    // 测试套前置和后置操作
    static void SetUpTestCase();
    static void TearDownTestCase();

    // 测试用例前置和后置操作
    void SetUp();
    void TearDown();
};

void TransSessionFuncTest::SetUp() {}

void TransSessionFuncTest::TearDown() {}

void TransSessionFuncTest::SetUpTestCase()
{
    LOG("SetUp begin");
    TestSetUp();
    SoftBus_Test_Permission::AddPermission(DEF_PKG_NAME);
    SoftBus_Test_Permission::AddPermission(PKG_NAME_TWO);
    SoftBus_Test_Permission::AddPermission(PKG_NAME_THREE);

    int ret = RegisterDeviceStateDefCallback();
    EXPECT_EQ(SOFTBUS_OK, ret) << "call reg node state callback fail";

    ret = WiFiUtils::CheckIsConnectToDefault();
    ASSERT_EQ(SOFTBUS_OK, ret) << "connect to default failed";
    ret = CheckRemoteDeviceIsNull(BOOL_TRUE);
    ASSERT_EQ(SOFTBUS_OK, ret) << "get node fail,please check network";

    LOG("SetUp end");
}

void TransSessionFuncTest::TearDownTestCase()
{
    int ret = UnRegisterDeviceStateDefCallback();
    EXPECT_EQ(SOFTBUS_OK, ret) << "call unReg node state callback fail";

    TestTearDown();
    SoftBus_Test_Permission::RemovePermission(DEF_PKG_NAME);
    SoftBus_Test_Permission::RemovePermission(PKG_NAME_TWO);
    SoftBus_Test_Permission::RemovePermission(PKG_NAME_THREE);
}

/**
 * @tc.number           : SUB_Softbus_Trans_Session_Func_0100
 * @tc.name             : one Client creates SessionServer + 1, Max succeeds, 1
 * fails
 * @tc.desc             : Test session management
 * @tc.type             : FUNC
 * @tc.size             : MediumTest
 */
HWTEST_F(TransSessionFuncTest,
         SUB_Softbus_Trans_Session_Func_0100,
         TestSize.Level3)
{
    int ret;
    char sessionNames[][SESSION_NAME_SIZE_MAX] = {
        "com.communication.demo1.1", "com.communication.demo1.2",
        "com.communication.demo1.3", "com.communication.demo1.4",
        "com.communication.demo1.5", "com.communication.demo1.6",
        "com.communication.demo1.7", "com.communication.demo1.8"};
    for (int i = 0; i < MAX_SESSION_SERVER_NUM_CLIENT; i++) {
        ret = CreateSessionServer(DEF_PKG_NAME, sessionNames[i],
                                  GetSessionListenser4Data());
        EXPECT_EQ(SOFTBUS_OK, ret) << "CreateSS fail,i=" << i;
    }
    string sessionName = "max+1";
    ret = CreateSessionServer(DEF_PKG_NAME, sessionName.c_str(),
                              GetSessionListenser4Data());
    printf("CreateSS max+1, ret:%d \n", ret);
    EXPECT_NE(SOFTBUS_OK, ret) << "CreateSS max+1 success, expect fail";

    for (int i = 0; i < MAX_SESSION_SERVER_NUM_CLIENT; i++) {
        ret = RemoveSessionServer(DEF_PKG_NAME, sessionNames[i]);
        EXPECT_EQ(SOFTBUS_OK, ret) << "RemoveSS fail,i=" << i;
    }
    ret = RemoveSessionServer(DEF_PKG_NAME, sessionName.c_str());
    printf("RemoveSS max+1, ret:%d \n", ret);
    EXPECT_NE(SOFTBUS_OK, ret) << "RemoveSS max+1 success, expect fail";
}

/**
 * @tc.number : SUB_Softbus_Trans_Session_Func_0200
 * @tc.name     : OpenSession + 1, Max succeeds, 1 fails
 * @tc.desc       : Test session management
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(TransSessionFuncTest,
         SUB_Softbus_Trans_Session_Func_0200,
         TestSize.Level3)
{
    int ret;
    ret = CreateSessionServer(DEF_PKG_NAME, SESSION_NAME_DATA,
                              GetSessionListenser4Data());
    EXPECT_EQ(SOFTBUS_OK, ret) << "CreateSS[data] fail";
    ret = CreateSessionServer(DEF_PKG_NAME, SESSION_NAME_CTL,
                              GetSessionListenser4Ctl());
    EXPECT_EQ(SOFTBUS_OK, ret) << "CreateSS[ctrl] fail";

    int count = MAX_SESSION_NUM / 2;
    int sessionId4Data[count];
    int sessionId4Ctrl[count];
    char groupId[][4] = {"g1", "g2", "g3", "g4",
                                    "g5", "g6", "g7", "g8"};
    ret = OpenSessionBatch4Data(groupId, sessionId4Data, count);
    EXPECT_EQ(SOFTBUS_OK, ret) << "OpenSessionWithDiffGroupId4Data fail";
    ret = OpenSessionBatch4Ctl(groupId, sessionId4Ctrl, count);
    EXPECT_EQ(SOFTBUS_OK, ret) << "OpenSessionWithDiffGroupId4Ctl fail";

    // open max+1, expect fail
    int sessionId;
    sessionId =
        OpenSession(SESSION_NAME_DATA, SESSION_NAME_DATA, GetNetworkId(),
                    DEF_GROUP_ID, GetSessionAttr4Data());
    EXPECT_FALSE(sessionId >= SESSION_ID_MIN)
        << "call OpenSession[data] fail,sid=" << sessionId;

    // close session
    ret = CloseSessionBatch4Data(sessionId4Data, count);
    EXPECT_EQ(SOFTBUS_OK, ret) << "CloseSessionBatch4Data fail";
    ret = CloseSessionBatch4Ctl(sessionId4Ctrl, count);
    EXPECT_EQ(SOFTBUS_OK, ret) << "CloseSessionBatch4Ctl fail";

    // remove  session server
    ret = RemoveSessionServer(DEF_PKG_NAME, SESSION_NAME_DATA);
    EXPECT_EQ(SOFTBUS_OK, ret) << "RemoveSS[data] fail";
    ret = RemoveSessionServer(DEF_PKG_NAME, SESSION_NAME_CTL);
    EXPECT_EQ(SOFTBUS_OK, ret) << "RemoveSS[ctrl] fail";
}

/**
 * @tc.number : SUB_Softbus_Trans_Session_Func_0300
 * @tc.name   : Obtain DevicedName based on Sessionid
 * @tc.desc   : Test session management
 * @tc.type   : FUNC
 * @tc.size   : MediumTest
 */
HWTEST_F(TransSessionFuncTest,
         SUB_Softbus_Trans_Session_Func_0300,
         TestSize.Level3)
{
    int ret;
    ret = CreateSsAndOpenSession4Data();
    ASSERT_EQ(SOFTBUS_OK, ret) << "create Ss and openS[data] fail";

    int sessionId = GetCurrentSessionId4Data();
    char mySessionName[SESSION_NAME_SIZE_MAX] = {0};
    ret = GetMySessionName(sessionId, mySessionName, SESSION_NAME_SIZE_MAX);
    EXPECT_EQ(SOFTBUS_OK, ret)
        << "call GetMySessionName fail, sid:" << sessionId;
    EXPECT_STREQ(SESSION_NAME_DATA, mySessionName)
        << "my session name cmp fail";

    char peerSessionName[SESSION_NAME_SIZE_MAX] = {0};
    ret = GetPeerSessionName(sessionId, peerSessionName, SESSION_NAME_SIZE_MAX);
    EXPECT_EQ(SOFTBUS_OK, ret) << "call GetPeerSessionName fail";
    EXPECT_STREQ(SESSION_NAME_DATA, peerSessionName)
        << "peer session name cmp fail";

    ret = CloseSessionAndRemoveSs4Data();
    EXPECT_EQ(SOFTBUS_OK, ret) << "close session and remove Ss fail";
}

/**
 * @tc.number : SUB_Softbus_Trans_Session_Func_0400
 * @tc.name     : Obtain DevicedId based on Sessionid
 * @tc.desc       : Test session management
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(TransSessionFuncTest,
         SUB_Softbus_Trans_Session_Func_0400,
         TestSize.Level3)
{
    int ret;
    ret = CreateSsAndOpenSession4Ctl();
    ASSERT_EQ(SOFTBUS_OK, ret) << "create Ss and openS[ctl] fail";

    char deviceId[SESSION_NAME_SIZE_MAX] = {0};
    ret = GetPeerDeviceId(GetCurrentSessionId4Ctl(), deviceId,
                          SESSION_NAME_SIZE_MAX);
    EXPECT_EQ(SOFTBUS_OK, ret) << "call GetPeerDeviceId fail";
    EXPECT_STREQ(GetNetworkId(), deviceId) << "peer device id cmp fail";

    ret = CloseSessionAndRemoveSs4Ctl();
    EXPECT_EQ(SOFTBUS_OK, ret) << "close session and remove Ss fail";
}
