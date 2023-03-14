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
#include "softbus_server_frame.h"

#define MAINLOOP_STACK_SIZE 5120
#define TEST_PKG_NAME "com.softbus.test"
#define DEFAULT_NODE_STATE_CB_NUM 9
#define ERRO_CAPDATA_LEN  514
#define DEFAULT_LOCAL_DEVICE_TYPE_ID  0
#define SLEEP_TIME 4
#include "cmsis_os.h"

#define UI_MAIN_TASK_DELAY 10000

static int32_t serverInit = 0;
static int32_t g_subscribeId = 0;
static int32_t g_publishId = 0;

LITE_TEST_SUIT(dsoftbus, buscenter, BusCenterTestSuite);

static void *BusCenterServerInit(void *arg)
{
    printf("----------start BusCenterServerInit-------------\n");
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

static BOOL BusCenterTestSuiteSetUp(void)
{
    printf("----------test case with DsoftbusMgrTest start-------------\n");
    if (GetServerIsInit() == false) {
        printf("----------Server Is not Init-------------\n");
        sleep(SLEEP_TIME);
    }
    return TRUE;
}

static BOOL BusCenterTestSuiteTearDown(void)
{
    printf("----------test case with DsoftbusMgrTest end-------------\n");
    return TRUE;
}

static int32_t GetPublishId(void)
{
    g_publishId++;
    return g_publishId;
}

static int32_t GetSubscribeId(void)
{
    g_subscribeId++;
    return g_subscribeId;
}

static void TestDeviceFound(const DeviceInfo *device)
{
    printf("[client]TestDeviceFound\n");
}

static void TestDiscoverResult(int32_t refreshId, RefreshResult reason)
{
    printf("[client]TestDiscoverResult:%d\n", reason);
}

static void OnJoinLNNDone(ConnectionAddr *addr, const char *networkId, int32_t retCode)
{
    (void)addr;
    (void)networkId;
    (void)retCode;
}

static void OnLeaveLNNDone(const char *networkId, int32_t retCode)
{
    (void)networkId;
    (void)retCode;
}

static void OnNodeOnline(NodeBasicInfo *info)
{
    (void)info;
}

static INodeStateCb g_nodeStateCb = {
    .events = EVENT_NODE_STATE_ONLINE,
    .onNodeOnline = OnNodeOnline,
};

static void TestPublishResult(int publishId, PublishResult reason)
{
    printf("[client]TestPublishResult:%d\n", reason);
}

static void OnTimeSyncResult(const TimeSyncResultInfo *info, int32_t retCode)
{
    (void)info;
    (void)retCode;
}

static ITimeSyncCb g_timeSyncCb = {
    .onTimeSyncResult = OnTimeSyncResult,
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

static IPublishCb g_publishCb = {
    .OnPublishResult = TestPublishResult
};

static IRefreshCallback g_refreshCb = {
    .OnDeviceFound = TestDeviceFound,
    .OnDiscoverResult = TestDiscoverResult
};

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

static SubscribeInfo g_sInfo1 = {
    .subscribeId = 1,
    .mode = DISCOVER_MODE_ACTIVE,
    .medium = COAP,
    .freq = MID,
    .isSameAccount = true,
    .isWakeRemote = false,
    .capability = "hicall",
    .capabilityData = NULL,
    .dataLen = 0,
};

/**
 * @tc.number    : testJoinLNN0001
 * @tc.name      : parameter is NULL
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(BusCenterTestSuite, testJoinLNN0001, Function | MediumTest | Level0)
{
    printf("------start testJoinLNN------\n");
    ConnectionAddr addr;

    while (GetServerIsInit() == false) {
        sleep(1);
    }
    osDelay(UI_MAIN_TASK_DELAY);
    int32_t ret = JoinLNN(NULL, &addr, OnJoinLNNDone);
    TEST_ASSERT_TRUE(ret == SOFTBUS_INVALID_PARAM);
    ret = JoinLNN(TEST_PKG_NAME, NULL, OnJoinLNNDone);
    TEST_ASSERT_TRUE(ret == SOFTBUS_INVALID_PARAM);
    ret = JoinLNN(TEST_PKG_NAME, &addr, NULL);
    TEST_ASSERT_TRUE(ret == SOFTBUS_INVALID_PARAM);
    printf("------end testJoinLNN------\n");
}

/**
 * @tc.number    : testLeaveLNN001
 * @tc.name      : bus center LeaveLNN interface exception test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(BusCenterTestSuite, testLeaveLNN001, Function | MediumTest | Level0)
{
    printf("------start testLeaveLNN001------\n");
    const char *errNetIdLenMore = "012345678998765432100123456789987654321001234567899876543210abcde";
    const char *networkId = "0123456789987654321001234567899876543210012345678998765432100123";

    BusCenterClientInit();

    int32_t ret = LeaveLNN(NULL, OnLeaveLNNDone);
    TEST_ASSERT_TRUE(ret == SOFTBUS_INVALID_PARAM);
    ret = LeaveLNN(networkId, NULL);
    TEST_ASSERT_TRUE(ret == SOFTBUS_INVALID_PARAM);
    ret = LeaveLNN(errNetIdLenMore, OnLeaveLNNDone);
    TEST_ASSERT_TRUE(ret != SOFTBUS_OK);
    printf("------end testLeaveLNN001------\n");
}

/*
 * @tc.name: testRegNodeDeviceStateCb001
 * @tc.desc: get all node info interface test
 * @tc.type: FUNC
 * @tc.require: AR000FK6J4
 */
LITE_TEST_CASE(BusCenterTestSuite, testRegNodeDeviceStateCb001, Function | MediumTest | Level0)
{
    int32_t ret = RegNodeDeviceStateCb(TEST_PKG_NAME, &g_nodeStateCb);
    TEST_ASSERT_TRUE(ret == SOFTBUS_OK);
    ret = UnregNodeDeviceStateCb(&g_nodeStateCb);
    TEST_ASSERT_TRUE(ret == SOFTBUS_OK);
}

/*
 * @tc.name: testRegNodeDeviceStateCb002
 * @tc.desc: get all node info interface test
 * @tc.type: FUNC
 * @tc.require: AR000FK6J4
 */
LITE_TEST_CASE(BusCenterTestSuite, testRegNodeDeviceStateCb002, Function | MediumTest | Level0)
{
    int32_t i;
    int32_t ret = 0;
    for (i = 0; i < DEFAULT_NODE_STATE_CB_NUM; ++i) {
        ret = RegNodeDeviceStateCb(TEST_PKG_NAME, &g_nodeStateCb);
        TEST_ASSERT_TRUE(ret == SOFTBUS_OK);
    }

    for (i = 0; i < DEFAULT_NODE_STATE_CB_NUM; ++i) {
        ret = UnregNodeDeviceStateCb(&g_nodeStateCb);
        TEST_ASSERT_TRUE(ret == SOFTBUS_OK);
    }
}

/*
 * @tc.name: testGetAllNodeDeviceInfo001
 * @tc.desc: get all node info interface test
 * @tc.type: FUNC
 * @tc.require: AR000FK6J4
 */
LITE_TEST_CASE(BusCenterTestSuite, testGetAllNodeDeviceInfo001, Function | MediumTest | Level0)
{
    printf("------start testGetAllNodeDeviceInfo001------\n");
    NodeBasicInfo *info = NULL;
    int infoNum;

    TEST_ASSERT_TRUE(GetAllNodeDeviceInfo(TEST_PKG_NAME, &info, &infoNum) == SOFTBUS_OK);
    TEST_ASSERT_TRUE(info == NULL);
    TEST_ASSERT_TRUE(infoNum == 0);
    if (info != NULL) {
        FreeNodeInfo(info);
    }
    printf("------end testGetAllNodeDeviceInfo001------\n");
}

/*
 * @tc.name: testGetLocalNodeDeviceInfo001
 * @tc.desc: get local info interface test
 * @tc.type: FUNC
 * @tc.require: AR000FK6J4
 */
LITE_TEST_CASE(BusCenterTestSuite, testGetLocalNodeDeviceInfo001, Function | MediumTest | Level0)
{
    printf("------start testGetLocalNodeDeviceInfo------\n");
    NodeBasicInfo info;
    int32_t ret = GetLocalNodeDeviceInfo(TEST_PKG_NAME, &info);
    TEST_ASSERT_TRUE(ret == SOFTBUS_OK);
    ret = strlen(info.networkId);
    TEST_ASSERT_TRUE(ret == (NETWORK_ID_BUF_LEN - 1));
    TEST_ASSERT_TRUE(info.deviceTypeId == DEFAULT_LOCAL_DEVICE_TYPE_ID);
    printf("------end testGetLocalNodeDeviceInfo------\n");
}

/*
 * @tc.name: testGetNodeKeyInfo001
 * @tc.desc: get node key info interface test
 * @tc.type: FUNC
 * @tc.require: AR000FK6J4
 */
LITE_TEST_CASE(BusCenterTestSuite, testGetNodeKeyInfo001, Function | MediumTest | Level0)
{
    NodeBasicInfo info;
    char uuid[UUID_BUF_LEN] = {0};
    char udid[UDID_BUF_LEN] = {0};

    (void)memset_s(&info, sizeof(NodeBasicInfo), 0, sizeof(NodeBasicInfo));
    TEST_ASSERT_TRUE(GetLocalNodeDeviceInfo(TEST_PKG_NAME, &info) == SOFTBUS_OK);
    TEST_ASSERT_TRUE(GetNodeKeyInfo(TEST_PKG_NAME, info.networkId, NODE_KEY_UDID,
        (uint8_t *)udid, UDID_BUF_LEN) == SOFTBUS_OK);
    TEST_ASSERT_TRUE(strlen(uuid) != (UUID_BUF_LEN - 1));
}


/*
 * @tc.name: testStartTimeSync001
 * @tc.desc: start time sync interface test
 * @tc.type: FUNC
 * @tc.require: AR000FN60G
 */
LITE_TEST_CASE(BusCenterTestSuite, testStartTimeSync001, Function | MediumTest | Level0)
{
    char networkId[] = "0123456789987654321001234567899876543210012345678998765432100123";

    TEST_ASSERT_TRUE(StartTimeSync(NULL, networkId, LOW_ACCURACY, SHORT_PERIOD, &g_timeSyncCb) != SOFTBUS_OK);
    TEST_ASSERT_TRUE(StartTimeSync(TEST_PKG_NAME, NULL, LOW_ACCURACY, SHORT_PERIOD, &g_timeSyncCb) != SOFTBUS_OK);
    TEST_ASSERT_TRUE(StartTimeSync(TEST_PKG_NAME, networkId, LOW_ACCURACY, SHORT_PERIOD, NULL) != SOFTBUS_OK);
}

/*
 * @tc.name: testStartTimeSync002
 * @tc.desc: start time sync interface test
 * @tc.type: FUNC
 * @tc.require: AR000FN60G
 */
LITE_TEST_CASE(BusCenterTestSuite, testStartTimeSync002, Function | MediumTest | Level0)
{
    char networkId[] = "0123456789987654321001234567899876543210012345678998765432100123";

    TEST_ASSERT_TRUE(StopTimeSync(NULL, networkId) != SOFTBUS_OK);
    TEST_ASSERT_TRUE(StopTimeSync(TEST_PKG_NAME, NULL) != SOFTBUS_OK);
}

/**
 * @tc.name: testPublishLNN001
 * @tc.desc: Verify wrong parameter
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(BusCenterTestSuite, testPublishLNN001, Function | MediumTest | Level0)
{
    int32_t ret = PublishLNN(NULL, &g_pInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);

    ret = PublishLNN(TEST_PKG_NAME, NULL, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);

    ret = PublishLNN(TEST_PKG_NAME, &g_pInfo, NULL);
    TEST_ASSERT_TRUE(ret != 0);

    g_pInfo.medium = (ExchanageMedium)(COAP + 1);
    ret = PublishLNN(TEST_PKG_NAME, &g_pInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_pInfo.medium = COAP;

    g_pInfo.mode = (DiscoverMode)(DISCOVER_MODE_ACTIVE + 1);
    ret = PublishLNN(TEST_PKG_NAME, &g_pInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_pInfo.mode = DISCOVER_MODE_ACTIVE;

    g_pInfo.freq = (ExchangeFreq)(SUPER_HIGH + 1);
    ret = PublishLNN(TEST_PKG_NAME, &g_pInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_pInfo.freq = LOW;

    g_pInfo.capabilityData = NULL;
    ret = PublishLNN(TEST_PKG_NAME, &g_pInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_pInfo.capabilityData = (unsigned char *)"capdata1";

    g_pInfo.dataLen = ERRO_CAPDATA_LEN;
    ret = PublishLNN(TEST_PKG_NAME, &g_pInfo, &g_publishCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_pInfo.dataLen = sizeof("capdata1");
}

/**
 * @tc.name: testRefreshLNN001
 * @tc.desc: Verify wrong parameter
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(BusCenterTestSuite, testRefreshLNN001, Function | MediumTest | Level0)
{
    int ret;

    ret = RefreshLNN(NULL, &g_sInfo, &g_refreshCb);
    TEST_ASSERT_TRUE(ret != 0);

    ret = RefreshLNN(TEST_PKG_NAME, NULL, &g_refreshCb);
    TEST_ASSERT_TRUE(ret != 0);

    ret = RefreshLNN(TEST_PKG_NAME, &g_sInfo, NULL);
    TEST_ASSERT_TRUE(ret != 0);

    g_sInfo.medium = (ExchanageMedium)(COAP + 1);
    ret = RefreshLNN(TEST_PKG_NAME, &g_sInfo, &g_refreshCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_sInfo.medium = COAP;

    g_sInfo.mode = (DiscoverMode)(DISCOVER_MODE_ACTIVE + 1);
    ret = RefreshLNN(TEST_PKG_NAME, &g_sInfo, &g_refreshCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_sInfo.mode = DISCOVER_MODE_ACTIVE;

    g_sInfo.freq = (ExchangeFreq)(SUPER_HIGH + 1);
    ret = RefreshLNN(TEST_PKG_NAME, &g_sInfo, &g_refreshCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_sInfo.freq = LOW;

    g_sInfo.capabilityData = NULL;
    ret = RefreshLNN(TEST_PKG_NAME, &g_sInfo, &g_refreshCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_sInfo.capabilityData = (unsigned char *)"capdata1";

    g_sInfo.dataLen = ERRO_CAPDATA_LEN;
    ret = RefreshLNN(TEST_PKG_NAME, &g_sInfo, &g_refreshCb);
    TEST_ASSERT_TRUE(ret != 0);
    g_sInfo.dataLen = sizeof("capdata1");
}

/*
 * @tc.name: testActiveMetaNode001
 * @tc.desc: meta node interface test
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(BusCenterTestSuite, testActiveMetaNode001, Function | MediumTest | Level0)
{
    char udid[] = "0123456789987654321001234567899876543210012345678998765432100123";
    char metaNodeId[NETWORK_ID_BUF_LEN] = {0};
    MetaNodeInfo infos[MAX_META_NODE_NUM];
    int32_t infoNum = MAX_META_NODE_NUM;
    MetaNodeConfigInfo configInfo;

    configInfo.addrNum = 1;
    TEST_ASSERT_TRUE(DeactiveMetaNode(TEST_PKG_NAME, metaNodeId) != SOFTBUS_OK);
    TEST_ASSERT_TRUE(strncpy_s(configInfo.udid, UDID_BUF_LEN, udid, UDID_BUF_LEN) == EOK);
    TEST_ASSERT_TRUE(ActiveMetaNode(TEST_PKG_NAME, &configInfo, metaNodeId) != SOFTBUS_OK);
    TEST_ASSERT_TRUE((int32_t)strlen(metaNodeId) != NETWORK_ID_BUF_LEN - 1);
    TEST_ASSERT_TRUE(ActiveMetaNode(TEST_PKG_NAME, &configInfo, metaNodeId) != SOFTBUS_OK);
    TEST_ASSERT_TRUE(GetAllMetaNodeInfo(TEST_PKG_NAME, infos, &infoNum) != SOFTBUS_OK);
    TEST_ASSERT_TRUE(infoNum != 1);
    TEST_ASSERT_TRUE(infos[0].isOnline != 0);
    TEST_ASSERT_TRUE(strcmp(infos[0].metaNodeId, metaNodeId) != 0);
    TEST_ASSERT_TRUE(strcmp(infos[0].configInfo.udid, udid) != 0);
    TEST_ASSERT_TRUE(infos[0].configInfo.addrNum != 1);
    TEST_ASSERT_TRUE(DeactiveMetaNode(TEST_PKG_NAME, metaNodeId) != SOFTBUS_OK);
    TEST_ASSERT_TRUE(GetAllMetaNodeInfo(TEST_PKG_NAME, infos, &infoNum) != SOFTBUS_OK);
    TEST_ASSERT_TRUE(infoNum != 0);
}

/*
 * @tc.name: testGetAllMetaNodeInfo001
 * @tc.desc: meta node interface test
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(BusCenterTestSuite, testGetAllMetaNodeInfo001, Function | MediumTest | Level0)
{
    char udid[] = "0123456789987654321001234567899876543210012345678998765432100123";
    char metaNodeId[NETWORK_ID_BUF_LEN] = {0};
    MetaNodeInfo infos[MAX_META_NODE_NUM];
    int32_t infoNum = MAX_META_NODE_NUM;
    MetaNodeConfigInfo configInfo;
    int i;

    configInfo.addrNum = 1;
    TEST_ASSERT_TRUE(strncpy_s(configInfo.udid, UDID_BUF_LEN, udid, UDID_BUF_LEN) == EOK);
    for (i = 0; i <= MAX_META_NODE_NUM; ++i) {
        configInfo.udid[0] += 1;
        if (i < MAX_META_NODE_NUM) {
            TEST_ASSERT_TRUE(ActiveMetaNode(TEST_PKG_NAME, &configInfo, metaNodeId) != SOFTBUS_OK);
            }
    }
    TEST_ASSERT_TRUE(GetAllMetaNodeInfo(TEST_PKG_NAME, infos, &infoNum) != SOFTBUS_OK);
    TEST_ASSERT_TRUE(infoNum == MAX_META_NODE_NUM);
    for (i = 0; i < MAX_META_NODE_NUM; ++i) {
        TEST_ASSERT_TRUE(DeactiveMetaNode(TEST_PKG_NAME, infos[i].metaNodeId) != SOFTBUS_OK);
    }
}

RUN_TEST_SUITE(BusCenterTestSuite);