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
#include <ctime>
#include <gtest/gtest.h>
#include <securec.h>
#include <sys/time.h>
#include <unistd.h>

#include "discovery_service.h"
#include "inner_session.h"
#include "session.h"
#include "softbus_utils.h"

using namespace testing::ext;

namespace OHOS {
enum TEST_PROCESS {
    TEST_INICIAL = 0,
    TEST_BEGIN,
    TEST_DEVICEFOUND,
    TEST_SESSIONOPEN,
    TEST_DATARECEIVE,
    TEST_SESSIONCLOSE,
};
const char *g_pkgName = "com.plrdtest";
const char *g_sessionName = "com.plrdtest.dsoftbus";
const char *g_testData = "{\"data\":\"open auth session test!!!\"}";
bool g_state = false;
int g_sessionId = -1;
int g_testCount = 0;
int g_testTimes = 0;
static void Wait(void);
static void Start(void);

ConnectionAddr g_addr;
ConnectionAddr g_addr1;
class BleAuthChannelPhoneTest : public testing::Test {
public:
    BleAuthChannelPhoneTest()
    {}
    ~BleAuthChannelPhoneTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override
    {}
    void TearDown() override
    {}
};

void BleAuthChannelPhoneTest::SetUpTestCase(void)
{
    printf("input test times:");
    if (scanf_s("%d", &g_testTimes, sizeof(g_testTimes)) < 0) {
        printf("input error!\n");
    }
    getchar();
}

void BleAuthChannelPhoneTest::TearDownTestCase(void)
{}

static SubscribeInfo g_sInfo = {
    .subscribeId = 1,
    .medium = BLE,
    .mode = DISCOVER_MODE_ACTIVE,
    .freq = MID,
    .capability = "dvKit",
    .capabilityData = (unsigned char *)"capdata3",
    .dataLen = sizeof("capdata3"),
    .isSameAccount = false,
    .isWakeRemote = false
};

static void TestDeviceFound(const DeviceInfo *device)
{
    if (ConvertBtMacToStr(g_addr.info.ble.bleMac, 18, (const uint8_t *)&(device->addr[0].info.ble.bleMac[0]), 6) != 0) {
        return;
    }
    if (strcmp(g_addr1.info.ble.bleMac, g_addr.info.ble.bleMac) != 0) {
        EXPECT_TRUE(g_testCount == TEST_BEGIN);
        g_testCount++;
        if (strcpy_s(g_addr1.info.ble.bleMac, BT_MAC_LEN, g_addr.info.ble.bleMac) != EOK) {
            return;
        }
        printf("[client]TestDeviceFound\r\n");
        printf("addr mac = %s\r\n", g_addr1.info.ble.bleMac);
        printf("account = %s\r\n", device->hwAccountHash);
        g_state = true;
    }
}

static void TestDiscoverFailed(int subscribeId, DiscoveryFailReason failReason)
{
    printf("[test]TestDiscoverFailed\r\n");
}

static void TestDiscoverySuccess(int subscribeId)
{
    printf("[test]TestDiscoverySuccess\r\n");
}

static void TestPublishSuccess(int publishId)
{
    printf("[test]TestPublishSuccess\r\n");
}

static void TestPublishFail(int publishId, PublishFailReason reason)
{
    printf("[test]TestPublishFail\r\n");
}

static IDiscoveryCallback g_subscribeCb = {
    .OnDeviceFound = TestDeviceFound,
    .OnDiscoverFailed = TestDiscoverFailed,
    .OnDiscoverySuccess = TestDiscoverySuccess
};

static IPublishCallback g_publishCb = {
    .OnPublishSuccess = TestPublishSuccess,
    .OnPublishFail = TestPublishFail
};

static int OnSessionOpened(int sessionId, int result)
{
    printf("[test]session opened,sesison id = %d\r\n", sessionId);
    EXPECT_TRUE(g_sessionId == sessionId);
    EXPECT_TRUE(g_testCount == TEST_DEVICEFOUND);
    g_testCount++;
    Start();
    return 0;
}

static void OnSessionClosed(int sessionId)
{
    printf("[test]session closed, session id = %d\r\n", sessionId);
}

static void OnBytesReceived(int sessionId, const void *data, unsigned int len)
{
    printf("[test]session bytes received, session id = %d data =%s\r\n", sessionId, data);
}

static void OnMessageReceived(int sessionId, const void *data, unsigned int len)
{
    printf("[test]session msg received, session id = %d data =%s\r\n", sessionId, data);
}

static ISessionListener g_sessionlistener = {
    .OnSessionOpened = OnSessionOpened,
    .OnSessionClosed = OnSessionClosed,
    .OnBytesReceived = OnBytesReceived,
    .OnMessageReceived = OnMessageReceived,
};

static void Wait(void)
{
    printf("[test]wait enter...\r\n");
    do {
        sleep(1);
    } while (!g_state);
    printf("[test]wait end!\r\n");
    g_state = false;
}

static void Start(void)
{
    g_state = true;
}

static int32_t TestStartDiscovery()
{
    printf("[test]TestStartDiscovery enter\r\n");
    g_sInfo.mode = DISCOVER_MODE_ACTIVE;
    int32_t ret = StartDiscovery(g_pkgName, &g_sInfo, &g_subscribeCb);
    EXPECT_TRUE(ret == 0);
    printf("[test]TestStartDiscovery end\r\n");
    return ret;
}

static int32_t TestCreateSessionServer()
{
    printf("[test]TestCreateSessionServer enter\r\n");
    int32_t ret = CreateSessionServer(g_pkgName, g_sessionName, &g_sessionlistener);
    EXPECT_TRUE(ret == 0);
    printf("[test]TestCreateSessionServer end\r\n");
    return ret;
}

static int32_t TestOpenSession()
{
    printf("[test]TestOpenSession enter\r\n");
    g_addr1.type = CONNECTION_ADDR_BLE;
    int32_t ret = OpenAuthSession(g_sessionName, &g_addr1, 1, NULL);
    EXPECT_TRUE(ret >= 0);
    printf("[test]TestOpenSession end\r\n");
    return ret;
}

static int32_t TestSendData(const char *data, int32_t len)
{
    printf("[test]TestSendData enter\r\n");
    int32_t  ret = SendBytes(g_sessionId, data, len);
    EXPECT_TRUE(ret == 0);
    printf("[test]TestSendData end\r\n");
    return ret;
}

static void TestCloseSeeesion()
{
    printf("[test]TestCloseSession enter\n");
    if (g_sessionId > 0) {
        CloseSession(g_sessionId);
        g_sessionId = -1;
    }
    printf("[test]TestCloseSession end\n");
}

static int32_t TestRemoveSessionServer()
{
    printf("[test]TestRemoveSessionServer enter\r\n");
    int32_t ret = RemoveSessionServer(g_pkgName, g_sessionName);
    EXPECT_TRUE(ret == 0);
    printf("[test]TestRemoveSessionServer end\r\n");
    return ret;
}

/**
 * @tc.name: PublishServiceTest001
 * @tc.desc: Verify wrong parameter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BleAuthChannelPhoneTest, ProcessPhoneActive001, TestSize.Level0)
{
    int32_t ret;
    g_testCount = TEST_BEGIN;
    ret = TestStartDiscovery();
    EXPECT_TRUE(ret == 0);
    Wait();
    ret = TestCreateSessionServer();
    EXPECT_TRUE(ret == 0);
    EXPECT_TRUE(g_testCount == TEST_DEVICEFOUND);
    for (int i = 0; i < g_testTimes; i++) {
        g_testCount = TEST_DEVICEFOUND;
        g_sessionId = TestOpenSession();
        EXPECT_TRUE(g_sessionId >= 0);
        Wait();
        EXPECT_TRUE(g_testCount == TEST_SESSIONOPEN);
        ret = TestSendData(g_testData, strlen(g_testData) + 1);
        EXPECT_TRUE(ret == 0);
        sleep(3);
        TestCloseSeeesion();
        sleep(5);
    }
    ret = TestRemoveSessionServer();
    EXPECT_TRUE(ret == 0);
END:
    EXPECT_TRUE(TEST_INICIAL == 0);
};
}