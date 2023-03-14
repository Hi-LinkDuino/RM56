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

#include "session.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_feature_config.h"
#include "softbus_log.h"

using namespace testing::ext;

namespace OHOS {
const char *g_pkgName = "dms";
const char *g_sessionName = "ohos.distributedschedule.dms.test";
const char *g_networkid = "ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00";
const char *g_groupid = "TEST_GROUP_ID";
static SessionAttribute g_sessionAttr = {
    .dataType = TYPE_BYTES,
};

class TransTcpDirectTest : public testing::Test {
public:
    TransTcpDirectTest()
    {}
    ~TransTcpDirectTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override
    {}
    void TearDown() override
    {}
};

void TransTcpDirectTest::SetUpTestCase(void)
{}

void TransTcpDirectTest::TearDownTestCase(void)
{}

static int OnSessionOpened(int sessionId, int result)
{
    LOG_INFO("session opened,sesison id = %d\r\n", sessionId);
    return SOFTBUS_OK;
}

static void OnSessionClosed(int sessionId)
{
    LOG_INFO("session closed, session id = %d\r\n", sessionId);
}

static void OnBytesReceived(int sessionId, const void *data, unsigned int len)
{
    LOG_INFO("session bytes received, session id = %d\r\n", sessionId);
}

static void OnMessageReceived(int sessionId, const void *data, unsigned int len)
{
    LOG_INFO("session msg received, session id = %d\r\n", sessionId);
}

static ISessionListener g_sessionlistener = {
    .OnSessionOpened = OnSessionOpened,
    .OnSessionClosed = OnSessionClosed,
    .OnBytesReceived = OnBytesReceived,
    .OnMessageReceived = OnMessageReceived,
};

/**
 * @tc.name: CreateSessionServerTest001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransTcpDirectTest, CreateSessionServerTest001, TestSize.Level0)
{
    int ret;
    ret = CreateSessionServer(NULL, g_sessionName, &g_sessionlistener);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = CreateSessionServer(g_pkgName, NULL, &g_sessionlistener);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = CreateSessionServer(g_pkgName, g_sessionName, NULL);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = CreateSessionServer(g_pkgName, g_sessionName, &g_sessionlistener);
    EXPECT_EQ(SOFTBUS_OK, ret);

    ret = RemoveSessionServer(g_pkgName, g_sessionName);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/**
 * @tc.name: CreateSessionServerTest002
 * @tc.desc: extern module active publish, use the normal parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransTcpDirectTest, CreateSessionServerTest002, TestSize.Level0)
{
    int ret;
    ret = CreateSessionServer(g_pkgName, g_sessionName, &g_sessionlistener);
    EXPECT_EQ(SOFTBUS_OK, ret);

    ret = RemoveSessionServer(g_pkgName, g_sessionName);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/**
 * @tc.name: CreateSessionServerTest003
 * @tc.desc: extern module active publish, use the same normal parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransTcpDirectTest, CreateSessionServerTest003, TestSize.Level0)
{
    int ret;
    ret = CreateSessionServer(g_pkgName, g_sessionName, &g_sessionlistener);
    EXPECT_EQ(SOFTBUS_OK, ret);

    ret = CreateSessionServer(g_pkgName, g_sessionName, &g_sessionlistener);
    EXPECT_EQ(SOFTBUS_OK, ret);

    ret = RemoveSessionServer(g_pkgName, g_sessionName);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/**
 * @tc.name: CreateSessionServerTest004
 * @tc.desc: extern module active publish, create 9 sessionServer, succ 8, failed at 9th.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransTcpDirectTest, CreateSessionServerTest004, TestSize.Level0)
{
    int ret, i;
    char const *sessionName[MAX_SESSION_SERVER_NUMBER + 1] = {
        "ohos.distributedschedule.dms.test0",
        "ohos.distributedschedule.dms.test1",
        "ohos.distributedschedule.dms.test2",
        "ohos.distributedschedule.dms.test3",
        "ohos.distributedschedule.dms.test4",
        "ohos.distributedschedule.dms.test5",
        "ohos.distributedschedule.dms.test6",
        "ohos.distributedschedule.dms.test7",
        "ohos.distributedschedule.dms.test8"
    };

    for (i = 0; i < MAX_SESSION_SERVER_NUMBER; i++) {
        ret = CreateSessionServer(g_pkgName, sessionName[i], &g_sessionlistener);
        EXPECT_EQ(SOFTBUS_OK, ret);
    }
    ret = CreateSessionServer(g_pkgName, sessionName[i], &g_sessionlistener);
    EXPECT_EQ(SOFTBUS_INVALID_NUM, ret);

    for (i = 0; i < MAX_SESSION_SERVER_NUMBER; i++) {
        ret = RemoveSessionServer(g_pkgName, sessionName[i]);
        EXPECT_EQ(SOFTBUS_OK, ret);
    }
}

/**
 * @tc.name: RemoveSessionServerTest001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransTcpDirectTest, RemoveSessionServerTest001, TestSize.Level0)
{
    int ret;
    ret = RemoveSessionServer(NULL, g_sessionName);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = RemoveSessionServer(g_pkgName, NULL);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/**
 * @tc.name: RemoveSessionServerTest002
 * @tc.desc: extern module active publish, use the same parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransTcpDirectTest, RemoveSessionServerTest002, TestSize.Level0)
{
    int ret;
    ret = CreateSessionServer(g_pkgName, g_sessionName, &g_sessionlistener);
    EXPECT_EQ(SOFTBUS_OK, ret);

    ret = RemoveSessionServer(g_pkgName, g_sessionName);
    EXPECT_EQ(SOFTBUS_OK, ret);

    ret = RemoveSessionServer(g_pkgName, g_sessionName);
    EXPECT_EQ(SOFTBUS_ERR, ret);
}

/**
 * @tc.name: OpenSessionTest001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransTcpDirectTest, OpenSessionTest001, TestSize.Level0)
{
    int ret;
    g_sessionAttr.dataType = TYPE_BYTES;

    ret = OpenSession(NULL, g_sessionName, g_networkid, g_groupid, &g_sessionAttr);
    EXPECT_EQ(INVALID_SESSION_ID, ret);

    ret = OpenSession(g_sessionName, NULL, g_networkid, g_groupid, &g_sessionAttr);
    EXPECT_EQ(INVALID_SESSION_ID, ret);

    ret = OpenSession(g_sessionName, g_sessionName, NULL, g_groupid, &g_sessionAttr);
    EXPECT_EQ(INVALID_SESSION_ID, ret);

    ret = OpenSession(g_sessionName, g_sessionName, g_networkid, NULL, &g_sessionAttr);
    EXPECT_EQ(INVALID_SESSION_ID, ret);

    ret = OpenSession(g_sessionName, g_sessionName, g_networkid, g_groupid, NULL);
    EXPECT_EQ(INVALID_SESSION_ID, ret);

    g_sessionAttr.dataType = TYPE_BUTT;
    ret = OpenSession(g_sessionName, g_sessionName, g_networkid, g_groupid, &g_sessionAttr);
    g_sessionAttr.dataType = TYPE_BYTES;
    EXPECT_EQ(INVALID_SESSION_ID, ret);
}

/**
 * @tc.name: SendBytesTest001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransTcpDirectTest, SendBytesTest001, TestSize.Level0)
{
    int ret;
    int sessionId = 1;
    const char *data = "testdata";
    uint32_t len = strlen(data);
    uint32_t maxLen;

    ret = SendBytes(-1, data, len);
    EXPECT_EQ(SOFTBUS_TRANS_INVALID_SESSION_ID, ret);

    ret = SendBytes(sessionId, NULL, len);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SendBytes(sessionId, data, 0);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SoftbusGetConfig(SOFTBUS_INT_MAX_BYTES_LENGTH, (unsigned char *)&maxLen, sizeof(maxLen));
    ASSERT_EQ(SOFTBUS_OK, ret);
    ret = SendMessage(sessionId, data, maxLen + 1);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}

/**
 * @tc.name: SendMessageTest001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransTcpDirectTest, SendMessageTest001, TestSize.Level0)
{
    int ret;
    int sessionId = 1;
    const char *data = "testdata";
    uint32_t len = strlen(data);
    uint32_t maxLen;

    ret = SendMessage(-1, data, len);
    EXPECT_EQ(SOFTBUS_TRANS_INVALID_SESSION_ID, ret);

    ret = SendMessage(sessionId, NULL, len);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SendMessage(sessionId, data, 0);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);

    ret = SoftbusGetConfig(SOFTBUS_INT_MAX_MESSAGE_LENGTH, (unsigned char *)&maxLen, sizeof(maxLen));
    ASSERT_EQ(SOFTBUS_OK, ret);
    ret = SendMessage(sessionId, data, maxLen + 1);
    EXPECT_EQ(SOFTBUS_INVALID_PARAM, ret);
}
} // namespace OHOS