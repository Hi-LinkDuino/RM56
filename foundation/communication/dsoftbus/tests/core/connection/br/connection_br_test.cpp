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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <securec.h>

#include "common_list.h"
#include "softbus_conn_interface.h"
#include "softbus_conn_manager.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_feature_config.h"
#include "softbus_log.h"

static const uint32_t CONN_HEAD_SIZE = 24;
#define TEST_BR_MAC "24:DA:33:6A:06:EC"

static unsigned int g_connId = 0;
#define WAIT_CONNECTION_COUNT 8
#define WAIT_CONNECTION_SLEEP_TIME 1

using namespace testing::ext;

namespace OHOS {
void ConnectedCB(unsigned int connectionId, const ConnectionInfo *info)
{
    printf("recv remote ConnectedCB %u %d\r\n", connectionId, info->type);
    if (info->type == CONNECT_BR) {
        g_connId = connectionId;
    }
    return;
}

void DisConnectCB(unsigned int connectionId, const ConnectionInfo *info)
{
    printf("DconDisConnect %u\r\n", connectionId);
    return;
}

void DataReceivedCB(unsigned int connectionId, ConnModule moduleId, int64_t seq, char *data, int len)
{
    printf("DconDataReceived moduleId %d %s %d\r\n", moduleId, data, len);
    return;
}

void ConnectSuccessedCB(unsigned int requestId, unsigned int connectionId, const ConnectionInfo *info)
{
    printf("ConnectSuccessedCB %u\r\n", connectionId);
    g_connId = connectionId;
    return;
}

void ConnectFailedCB(unsigned int requestId, int reason)
{
    (void)requestId;
    (void)reason;
    printf("DconConnectFailed\r\n");
    return;
}

class ConnectionBrTest : public testing::Test {
public:
    ConnectionBrTest()
    {}
    ~ConnectionBrTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ConnectionBrTest::SetUpTestCase(void)
{
    SoftbusConfigInit();
    ConnServerInit();
}

void ConnectionBrTest::TearDownTestCase(void)
{}

void ConnectionBrTest::SetUp(void)
{}

void ConnectionBrTest::TearDown(void)
{}

/*
* @tc.name: testConnmanger001
* @tc.desc: test ConnTypeIsSupport
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(ConnectionBrTest, testConnmanger001, TestSize.Level1)
{
    int ret;
    printf("testConnmanger001\r\n");

    ret = ConnTypeIsSupport(CONNECT_BR);
    EXPECT_EQ(SOFTBUS_OK, ret);
};

/*
* @tc.name: testConnmanger002
* @tc.desc: test invalid param
* @tc.type: FUNC
* @tc.require:AR000GIIE9
*/
HWTEST_F(ConnectionBrTest, testConnmanger002, TestSize.Level1)
{
    printf("test begin testConnmanger002 \r\n");
    int ret;
    ret = ConnSetConnectCallback(static_cast<ConnModule>(0), nullptr);
    ASSERT_TRUE(ret != SOFTBUS_OK);
    ret = ConnConnectDevice(nullptr, 0, nullptr);
    ASSERT_TRUE(ret != SOFTBUS_OK);
    ret = ConnPostBytes(0, nullptr);
    ASSERT_TRUE(ret != SOFTBUS_OK);
    ret = ConnStartLocalListening(nullptr);
    ASSERT_TRUE(ret != SOFTBUS_OK);
    ret = ConnStopLocalListening(nullptr);
    ASSERT_TRUE(ret != SOFTBUS_OK);
    EXPECT_EQ(SOFTBUS_OK, SOFTBUS_OK);
};

/*
* @tc.name: testConnmanger003
* @tc.desc: test set unset callback and connect post disconnect
* @tc.type: FUNC
* @tc.require:AR000GIRGE
*/
HWTEST_F(ConnectionBrTest, testConnmanger003, TestSize.Level1)
{
    int ret;
    int reqId;
    ConnectCallback connCb;
    ConnectResult connRet;
    ConnPostData data;
    ConnectOption info;
    const char *str = "send msg local2\r\n";
    printf("test begin testConnmanger003 \r\n");

    connCb.OnConnected = ConnectedCB;
    connCb.OnDisconnected = DisConnectCB;
    connCb.OnDataReceived = DataReceivedCB;
    ret = ConnSetConnectCallback(MODULE_TRUST_ENGINE, &connCb);
    EXPECT_EQ(SOFTBUS_OK, ret);
    info.type = CONNECT_BR;
    (void)memcpy_s(info.info.brOption.brMac, BT_MAC_LEN, TEST_BR_MAC, BT_MAC_LEN);
    printf("brMac: %s\n", info.info.brOption.brMac);
    connRet.OnConnectFailed = ConnectFailedCB;
    connRet.OnConnectSuccessed = ConnectSuccessedCB;
    reqId = ConnGetNewRequestId(MODULE_TRUST_ENGINE);
    ret = ConnConnectDevice(&info, reqId, &connRet);
    EXPECT_EQ(SOFTBUS_OK, ret);
    if (g_connId) {
        data.buf = (char *)calloc(1, CONN_HEAD_SIZE + 20);
        ASSERT_TRUE(data.buf != NULL);
        (void)strcpy_s(data.buf + 1, strlen(str), str);
        data.len = CONN_HEAD_SIZE + 20;
        data.module = MODULE_TRUST_ENGINE;
        data.pid = 0;
        data.flag = 1;
        data.seq = 1;
        ret = ConnPostBytes(g_connId, &data);
        EXPECT_EQ(SOFTBUS_OK, ret);
        if (data.buf != nullptr) {
            free(data.buf);
        }
        ret = ConnDisconnectDevice(g_connId);
        EXPECT_EQ(SOFTBUS_OK, ret);
    }

    ConnUnSetConnectCallback(MODULE_TRUST_ENGINE);
    g_connId = 0;
};

/*
* @tc.name: testConnmanger004
* @tc.desc: test set unset callback and post disconnect without connect
* @tc.type: FUNC
* @tc.require:AR000GIIE9
*/
HWTEST_F(ConnectionBrTest, testConnmanger004, TestSize.Level1)
{
    printf("test begin ConnManagerTest004 \r\n");
    int ret;
    ConnectCallback connCb;
    LocalListenerInfo info;

    connCb.OnConnected = ConnectedCB;
    connCb.OnDisconnected = DisConnectCB;
    connCb.OnDataReceived = DataReceivedCB;
    ret = ConnSetConnectCallback(MODULE_TRUST_ENGINE, &connCb);
    EXPECT_EQ(SOFTBUS_OK, ret);
    info.type = CONNECT_BR;
    ret = ConnStartLocalListening(&info);
    EXPECT_EQ(SOFTBUS_OK, ret);
    ret = ConnStopLocalListening(&info);
    EXPECT_EQ(SOFTBUS_OK, ret);
    ConnUnSetConnectCallback(MODULE_TRUST_ENGINE);
    g_connId = 0;
};

/*
* @tc.name: testConnmanger005
* @tc.desc: test set unset callback and connect post disconnect
* @tc.type: FUNC
* @tc.require:AR000GIRGE
*/
HWTEST_F(ConnectionBrTest, testConnmanger005, TestSize.Level1)
{
    int reqId = 1;
    int ret;
    ConnectCallback connCb;
    ConnectOption optionInfo;
    ConnectionInfo info;
    ConnectResult connRet;

    connCb.OnConnected = ConnectedCB;
    connCb.OnDisconnected = DisConnectCB;
    connCb.OnDataReceived = DataReceivedCB;
    ret = ConnSetConnectCallback(MODULE_TRUST_ENGINE, &connCb);
    EXPECT_EQ(SOFTBUS_OK, ret);

    optionInfo.type = CONNECT_BR;
    (void)memcpy_s(optionInfo.info.brOption.brMac, BT_MAC_LEN, TEST_BR_MAC, BT_MAC_LEN);
    printf("brMac: %s\n", optionInfo.info.brOption.brMac);
    connRet.OnConnectFailed = ConnectFailedCB;
    connRet.OnConnectSuccessed = ConnectSuccessedCB;
    reqId = ConnGetNewRequestId(MODULE_TRUST_ENGINE);
    ret = ConnConnectDevice(&optionInfo, reqId, &connRet);
    EXPECT_EQ(SOFTBUS_OK, ret);
    if (g_connId) {
        ret = ConnGetConnectionInfo(g_connId, &info);
        EXPECT_EQ(SOFTBUS_OK, ret);
        ret = ConnDisconnectDeviceAllConn(&optionInfo);
        g_connId = 0;
        EXPECT_EQ(SOFTBUS_OK, ret);
        printf("testConnmanger005 ConnDisconnectDevice\r\n");
    }
    printf("testConnmanger005 ConnUnSetConnectCallback\r\n");
    ConnUnSetConnectCallback(MODULE_TRUST_ENGINE);
    printf("testConnmanger005 ConnUnSetConnectCallback end 11\r\n");
};
} // namespace OHOS
