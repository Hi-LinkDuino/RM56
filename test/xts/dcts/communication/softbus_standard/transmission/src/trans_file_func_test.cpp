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

static int g_waitFlag = WAIT_DEF_VALUE;

static int FileSessionOpened(int sessionId, int result)
{
    LOG("[cb][file]open session sid[%d],rst[%d]", sessionId, result);
    if (result == SOFTBUS_OK) {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    } else {
        g_waitFlag = WAIT_FAIL_VALUE;
    }
    return SOFTBUS_OK;
}

static void FileSessionClosed(int sessionId)
{
    LOG("[cb][file]close session sid[%d]", sessionId);
}

static void FileBytesReceived(int sessionId,
                              const void* data,
                              unsigned int dataLen)
{
    LOG("[cb][file]ByteRec sid:%d, data len:%d", sessionId, dataLen);
    if (data == NULL) {
        LOG("[cb][file]ByteRec invalid data=null sid[%d]", sessionId);
        g_waitFlag = WAIT_FAIL_VALUE;
    } else {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    }
}

static void FileMessageReceived(int sessionId,
                                const void* data,
                                unsigned int dataLen)
{
    LOG("[cb][file]MessageRec sid:%d, data len:%d", sessionId, dataLen);
    if (data == NULL) {
        LOG("[cb][file]MessageRec invalid data=null sid[%d]", sessionId);
        g_waitFlag = WAIT_FAIL_VALUE;
    } else {
        g_waitFlag = WAIT_SUCCESS_VALUE;
    }
}

static SessionAttribute g_fileSessionAttr = {
    .dataType = TYPE_FILE,
};

static ISessionListener g_fileSessionListener = {
    .OnSessionOpened = FileSessionOpened,
    .OnSessionClosed = FileSessionClosed,
    .OnBytesReceived = FileBytesReceived,
    .OnMessageReceived = FileMessageReceived,
};
static const char* g_fileOne[] = {
    "/data/SoftBusNetTest_BindSelf_1.xml",
};

using namespace testing::ext;

class TransFileFuncTest : public testing::Test {
public:
    // 测试套前置和后置操作
    static void SetUpTestCase();
    static void TearDownTestCase();

    // 测试用例前置和后置操作
    void SetUp();
    void TearDown();
};

void TransFileFuncTest::SetUp() {}

void TransFileFuncTest::TearDown() {}

void TransFileFuncTest::SetUpTestCase()
{
    LOG("SetUp begin");
    TestSetUp();
    SoftBus_Test_Permission::AddPermission(DEF_PKG_NAME);
    int ret = RegisterDeviceStateDefCallback();
    EXPECT_EQ(SOFTBUS_OK, ret) << "call reg node state callback fail";

    ret = CheckRemoteDeviceIsNull(BOOL_TRUE);
    ASSERT_EQ(SOFTBUS_OK, ret) << "get node fail,please check network";

    // set listener
    ret = SetFileSendListener(DEF_PKG_NAME, SESSION_NAME_FILE,
                              GetSendFileListener());
    EXPECT_EQ(SOFTBUS_OK, ret) << "call SetFileSendListener fail";
    ret = SetFileReceiveListener(DEF_PKG_NAME, SESSION_NAME_FILE,
                                 GetRecvFileListener(), RECV_FILE_PATH);
    EXPECT_EQ(SOFTBUS_OK, ret) << "call SetFileSendListener fail";

    LOG("SetUp end");
}

void TransFileFuncTest::TearDownTestCase()
{
    int ret = UnRegisterDeviceStateDefCallback();
    EXPECT_EQ(SOFTBUS_OK, ret) << "call unReg node state callback fail";

    TestTearDown();
    SoftBus_Test_Permission::RemovePermission(DEF_PKG_NAME);
}

static int WaitFile(int timeout)
{
    LOG("start waitfile,timeout:%d", timeout);
    int count = 0;
    int t = timeout;
    while (t > 0) {
        sleep(1);
        if (g_waitFlag != WAIT_DEF_VALUE) {
            LOG("waitfile success[flag:%d][time:%d]", g_waitFlag, count);
            break;
        }
        t--;
        count++;
    }
    if (g_waitFlag != WAIT_SUCCESS_VALUE) {
        LOG("waitfile fail[exp:%d, real:%d][used time:%d]", WAIT_SUCCESS_VALUE,
            g_waitFlag, count);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

/**
 * @tc.number : SUB_Softbus_Trans_SendFile_Func_0100
 * @tc.name     : test SendFile
 * @tc.desc       : Test send file function
 * @tc.type       : FUNC
 * @tc.size        : MediumTest
 */
HWTEST_F(TransFileFuncTest,
         SUB_Softbus_Trans_SendFile_Func_0100,
         TestSize.Level2)
{
    int ret;
    ret = CreateSessionServer(DEF_PKG_NAME, SESSION_NAME_FILE,
                              &g_fileSessionListener);
    EXPECT_EQ(SOFTBUS_OK, ret) << "CreateSS[file] fail";

    g_waitFlag = WAIT_DEF_VALUE;
    int sid = OpenSession(SESSION_NAME_FILE, SESSION_NAME_FILE, GetNetworkId(),
                          DEF_GROUP_ID, &g_fileSessionAttr);
    EXPECT_TRUE(sid >= SESSION_ID_MIN)
        << "call OpenSession[file] fail, sid=" << sid;
    if (sid >= SESSION_ID_MIN) {
        ret = WaitFile(10);
        EXPECT_EQ(SOFTBUS_OK, ret) << "wait opensession fail[file]";
    }

    ResetWaitFlag();
    ret = SendFile(sid, g_fileOne, NULL, 1);
    EXPECT_EQ(SOFTBUS_OK, ret) << "call SendFile fail";
    ret = Wait(10);
    EXPECT_EQ(SOFTBUS_OK, ret) << "wait send fail rst fail";

    CloseSession(sid);
    ret = RemoveSessionServer(DEF_PKG_NAME, SESSION_NAME_FILE);
    EXPECT_EQ(SOFTBUS_OK, ret) << "RemoveSS[proxy] fail";
}
