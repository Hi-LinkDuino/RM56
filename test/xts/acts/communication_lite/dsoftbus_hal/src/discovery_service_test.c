/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "hctest.h"
#include "securec.h"
#include "session.h"
#include "softbus_bus_center.h"
#include "softbus_errcode.h"
#include "discovery_service.h"
#include "softbus_server_frame.h"
#include "cmsis_os.h"

#define UI_MAIN_TASK_DELAY 10000

#define MAINLOOP_STACK_SIZE 5120
#define SLEEP_TIME 4
static int32_t serverInit = 0;
static int g_subscribeId = 0;
static int g_publishId = 0;
static const char *g_pkgName = "Softbus_Kits";

static const int32_t ERRO_CAPDATA_LEN = 514;

static int GetSubscribeId(void)
{
    g_subscribeId++;
    return g_subscribeId;
}

static int GetPublishId(void)
{
    g_publishId++;
    return g_publishId;
}

static SubscribeInfo g_sInfo = {
    .subscribeId = 1,
    .mode = DISCOVER_MODE_ACTIVE,
    .medium = COAP,
    .freq = MID,
    .isSameAccount = true,
    .isWakeRemote = false,
    .capability = "dvKit",
    .capabilityData = (unsigned char *)"capdata3",
    .dataLen = sizeof("capdata3")
};

static PublishInfo g_pInfo = {
    .publishId = 1,
    .mode = DISCOVER_MODE_ACTIVE,
    .medium = COAP,
    .freq = MID,
    .capability = "dvKit",
    .capabilityData = (unsigned char *)"capdata4",
    .dataLen = sizeof("capdata4")
};

static PublishInfo g_pInfo1 = {
    .publishId = 1,
    .mode = DISCOVER_MODE_ACTIVE,
    .medium = COAP,
    .freq = MID,
    .capability = "dvKit",
    .capabilityData = NULL,
    .dataLen = 0
};

static SubscribeInfo g_sInfo1 = {
    .subscribeId = 1,
    .mode = DISCOVER_MODE_ACTIVE,
    .medium = COAP,
    .freq = MID,
    .isSameAccount = true,
    .isWakeRemote = false,
    .capability = "hicall",
    .capabilityData = NULL,
    .dataLen = 0
};

static void TestDeviceFound(void)
{
    printf("[client]TestDeviceFound\n");
}

static void TestDiscoverFailed(void)
{
    printf("[client]TestDiscoverFailed\n");
}

static void TestDiscoverySuccess(void)
{
    printf("[client]TestDiscoverySuccess\n");
}

static void TestPublishSuccess(void)
{
    printf("[client]TestPublishSuccess\n");
}

static void TestPublishFail(void)
{
    printf("[client]TestPublishFail\n");
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

LITE_TEST_SUIT(dsoftbus, discoveryservice, DiscoveryServiceTestSuite);

static void *ServerInit(void *arg)
{
    printf("----------start ServerInit-------------\n");
    InitSoftBusServer();
    return NULL;
}

static void ThreadCreateTest(void *(*entry)(void *arg))
{
    pthread_t tid;
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setstacksize(&threadAttr, MAINLOOP_STACK_SIZE);
    pthread_create(&tid, &threadAttr, entry, 0);
}

static BOOL DiscoveryServiceTestSuiteSetUp(void)
{
    printf("----------test case with DiscoveryServiceTestSuite start-------------\n");
    if (GetServerIsInit() == false) {
        printf("----------Server Is not Init-------------\n");
        ThreadCreateTest(ServerInit);
        sleep(SLEEP_TIME);
    }
    serverInit = 1;
    return TRUE;
}

static BOOL DiscoveryServiceTestSuiteTearDown(void)
{
    printf("----------test case with DiscoveryServiceTestSuite end-------------\n");
    return TRUE;
}

/**
 * @tc.name: PublishServiceTest001
 * @tc.desc: Verify wrong parameter
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(DiscoveryServiceTestSuite, PublishServiceTest001, Function | MediumTest | Level0)
{
    int ret;
    PublishInfo testInfo = {
        .publishId = GetPublishId(),
        .mode = DISCOVER_MODE_ACTIVE,
        .medium = COAP,
        .freq = MID,
        .capability = "dvKit",
        .capabilityData = (unsigned char *)"capdata2",
        .dataLen = sizeof("capdata2")
    };

    while (GetServerIsInit() == false) {
        sleep(1);
    }
    osDelay(UI_MAIN_TASK_DELAY);
    ret = PublishService(NULL, &testInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);

    ret = PublishService(g_pkgName, NULL, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);

    ret = PublishService(g_pkgName, &testInfo, NULL);
    TEST_ASSERT_TRUE(ret != 0);

    testInfo.medium = (ExchanageMedium)(COAP + 1);
    ret = PublishService(g_pkgName, &testInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.medium = COAP;

    testInfo.mode = (DiscoverMode)(DISCOVER_MODE_ACTIVE + 1);
    ret = PublishService(g_pkgName, &testInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.mode = DISCOVER_MODE_ACTIVE;

    testInfo.freq = (ExchangeFreq)(SUPER_HIGH + 1);
    ret = PublishService(g_pkgName, &testInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.freq = LOW;

    testInfo.capabilityData = NULL;
    ret = PublishService(g_pkgName, &testInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.capabilityData = (unsigned char *)"capdata1";

    testInfo.dataLen = ERRO_CAPDATA_LEN;
    ret = PublishService(g_pkgName, &testInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.dataLen = sizeof("capdata1");
}

/**
 * @tc.name: UnPublishServiceTest001
 * @tc.desc: Verify wrong parameter
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(DiscoveryServiceTestSuite, UnPublishServiceTest001, Function | MediumTest | Level0)
{
    int ret;
    int tmpId = GetPublishId();
    ret = UnPublishService(NULL, tmpId);
    TEST_ASSERT_TRUE(ret != 0);
}

/**
 * @tc.name: StartDiscoveryTest001
 * @tc.desc: Verify wrong parameter
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(DiscoveryServiceTestSuite, StartDiscoveryTest001, Function | MediumTest | Level0)
{
    int ret;
    SubscribeInfo testInfo = {
        .subscribeId = GetSubscribeId(),
        .mode = DISCOVER_MODE_ACTIVE,
        .medium = COAP,
        .freq = MID,
        .isSameAccount = true,
        .isWakeRemote = false,
        .capability = "dvKit",
        .capabilityData = (unsigned char *)"capdata3",
        .dataLen = sizeof("capdata3")
    };

    ret = StartDiscovery(NULL, &testInfo, &g_subscribeCb);
    TEST_ASSERT_TRUE(ret != 0);

    ret = StartDiscovery(g_pkgName, NULL, &g_subscribeCb);
    TEST_ASSERT_TRUE(ret != 0);

    ret = StartDiscovery(g_pkgName, &testInfo, NULL);
    TEST_ASSERT_TRUE(ret != 0);

    testInfo.medium = (ExchanageMedium)(COAP + 1);
    ret = StartDiscovery(g_pkgName, &testInfo, &g_subscribeCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.medium = COAP;

    testInfo.mode = (DiscoverMode)(DISCOVER_MODE_ACTIVE + 1);
    ret = StartDiscovery(g_pkgName, &testInfo, &g_subscribeCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.mode = DISCOVER_MODE_ACTIVE;

    testInfo.freq = (ExchangeFreq)(SUPER_HIGH + 1);
    ret = StartDiscovery(g_pkgName, &testInfo, &g_subscribeCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.freq = LOW;

    testInfo.capabilityData = NULL;
    ret = StartDiscovery(g_pkgName, &testInfo, &g_subscribeCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.capabilityData = (unsigned char *)"capdata1";

    testInfo.dataLen = ERRO_CAPDATA_LEN;
    ret = StartDiscovery(g_pkgName, &testInfo, &g_subscribeCb);
    TEST_ASSERT_TRUE(ret != 0);
    testInfo.dataLen = sizeof("capdata1");
}

/**
 * @tc.name: StopDiscoveryTest001
 * @tc.desc: Verify wrong parameter
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(DiscoveryServiceTestSuite, StopDiscoveryTest001, Function | MediumTest | Level0)
{
    int ret;
    int tmpId = GetSubscribeId();
    ret = StopDiscovery(NULL, tmpId);
    TEST_ASSERT_TRUE(ret != 0);
}

RUN_TEST_SUITE(DiscoveryServiceTestSuite);