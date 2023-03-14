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

#include "hctest.h"
#include "securec.h"
#include "session.h"
#include "softbus_def.h"
#include "softbus_config_type.h"
#include "softbus_bus_center.h"
#include "softbus_errcode.h"

#define TEST_PKG_NAME "com.softbus.test"
#define DEFAULT_NODE_STATE_CB_NUM 9
#define LOG2_DBG(fmt, ...) printf("DEBUG:%s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG2_INFO(fmt, ...) printf("INFO:%s:%d " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG2_WARN(fmt, ...) printf("WARN:%s:%d " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG2_ERR(fmt, ...)  printf("ERROR:%s:%d " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

enum StatusNum {
    TRANS_STATE_NONE = 0,
    TRANS_STATE_INIT,
    TRANS_STATE_CREATE_SESSION_SERVER,
    TRANS_SET_FILE_SEND_LISTENER,
    TRANS_STATE_OPEN,
    TRANS_STATE_SEND_BYTE,
    TRANS_STATE_SEND_MESSAGE,
    TRANS_STATE_SEND_FILE,
    TRANS_STATE_CLOSE,
    TRANS_STATE_REMOVE_SESSION_SERVER,
    TRANS_STATE_CREATE_PHONE = 10,
    TRANS_STATE_GET_SESSION_NAME,
    TRANS_STATE_GET_DEVICE_ID,
    TRANS_CLEAR_LOG,
    TRANS_TEST_FIN,

    LNN_STATE_JOINLNN = 20,
    LNN_STATE_LEAVELNN,
};

const char *g_pkgName = "dms";
const char *g_sessionName = "ohos.distributedschedule.dms.test";
const char *g_networkid = "ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00ABCDEF00";
const char *g_groupid = "TEST_GROUP_ID";
const char *g_testModuleName   = "com.plrdtest.dsoftbus";
const char *g_testSessionName = "com.plrdtest.dsoftbus.JtOnOpenFileSession";
const char *g_testSessionNamE2 = "com.plrdtest.dsoftbus.JtOpenFileSession";
static SessionAttribute g_sessionAttr = {
    .dataType = TYPE_BYTES,
};
const int FILE_NUM = 4;
int32_t g_sessionId = -1;
int32_t g_stateDebug = LNN_STATE_JOINLNN;

static void TestChangeDebugState(int32_t state)
{
    g_stateDebug = state;
    LOG2_INFO("change to debug state: %d", state);
}

static int OnSendFileProcess(int sessionId, uint64_t bytesUpload, uint64_t bytesTotal)
{
    LOG2_INFO("OnSendFileProcess sessionId = %d, bytesUpload = %llu, total = %llu\n",
        sessionId, bytesUpload, bytesTotal);
    return 0;
}

static int OnSendFileFinished(int sessionId, const char *firstFile)
{
    LOG2_INFO("OnSendFileFinished sessionId = %d, first file = %s\n", sessionId, firstFile);
    TestChangeDebugState(TRANS_STATE_CLOSE);
    return 0;
}

static void OnFileTransError(int sessionId)
{
    LOG2_INFO("OnFileTransError sessionId = %d\n", sessionId);
}

static IFileSendListener g_fileSendListener = {
    .OnSendFileProcess = OnSendFileProcess,
    .OnSendFileFinished = OnSendFileFinished,
    .OnFileTransError = OnFileTransError,
};

static ISessionListener g_sessionlistener = {
    .OnSessionOpened = 1,
};

static int OnReceiveFileStarted(int sessionId, const char *files, int fileCnt)
{
    LOG2_INFO("File receive start sessionId = %d, first file = %s, fileCnt = %d\n", sessionId, files, fileCnt);
    return 0;
}

static void OnReceiveFileFinished(int sessionId, const char *files, int fileCnt)
{
    LOG2_INFO("File receive finished sessionId = %d, first file = %s, fileCnt = %d\n", sessionId, files, fileCnt);
}

static IFileReceiveListener g_fileRecvListener = {
    .OnReceiveFileStarted = OnReceiveFileStarted,
    .OnReceiveFileFinished = OnReceiveFileFinished,
    .OnFileTransError = OnFileTransError,
};

LITE_TEST_SUIT(dsoftbus, session, SessionTestSuite);

static BOOL SessionTestSuiteSetUp(void)
{
    printf("----------test case with SessionTestSuite start-------------\n");
    return TRUE;
}

static BOOL SessionTestSuiteTearDown(void)
{
    printf("----------test case with SessionTestSuite end-------------\n");
    return TRUE;
}

/**
 * @tc.name: TestCreateSessionServer001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC99
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestCreateSessionServer001, Function | MediumTest | Level0)
{
    int ret;
    ret = CreateSessionServer(NULL, g_sessionName, &g_sessionlistener);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);

    ret = CreateSessionServer(g_pkgName, NULL, &g_sessionlistener);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);

    ret = CreateSessionServer(g_pkgName, g_sessionName, NULL);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);
}

/**
 * @tc.name: TestRemoveSessionServer001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestRemoveSessionServer001, Function | MediumTest | Level0)
{
    int ret;
    ret = RemoveSessionServer(NULL, g_sessionName);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);

    ret = RemoveSessionServer(g_pkgName, NULL);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);
}

/**
 * @tc.name: TestOpenSession001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestOpenSession001, Function | MediumTest | Level0)
{
    int ret;
    g_sessionAttr.dataType = TYPE_BYTES;

    ret = OpenSession(NULL, g_sessionName, g_networkid, g_groupid, &g_sessionAttr);
    TEST_ASSERT_EQUAL_INT(INVALID_SESSION_ID, ret);

    ret = OpenSession(g_sessionName, NULL, g_networkid, g_groupid, &g_sessionAttr);
    TEST_ASSERT_EQUAL_INT(INVALID_SESSION_ID, ret);

    ret = OpenSession(g_sessionName, g_sessionName, NULL, g_groupid, &g_sessionAttr);
    TEST_ASSERT_EQUAL_INT(INVALID_SESSION_ID, ret);

    ret = OpenSession(g_sessionName, g_sessionName, g_networkid, NULL, &g_sessionAttr);
    TEST_ASSERT_EQUAL_INT(INVALID_SESSION_ID, ret);

    ret = OpenSession(g_sessionName, g_sessionName, g_networkid, g_groupid, NULL);
    TEST_ASSERT_EQUAL_INT(INVALID_SESSION_ID, ret);
}

/**
 * @tc.name: TestSendBytes001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestSendBytes001, Function | MediumTest | Level0)
{
    int ret;
    int sessionId = 1;
    const char *data = "testdata";
    uint32_t len = strlen(data);
    uint32_t maxLen;

    ret = SendBytes(-1, data, len);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_TRANS_INVALID_SESSION_ID, ret);

    ret = SendBytes(sessionId, NULL, len);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);

    ret = SendBytes(sessionId, data, 0);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);
}

/**
 * @tc.name: TestSendMessage001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestSendMessage001, Function | MediumTest | Level0)
{
    int ret;
    int sessionId = 1;
    const char *data = "testdata";
    uint32_t len = strlen(data);
    uint32_t maxLen;

    ret = SendMessage(-1, data, len);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_TRANS_INVALID_SESSION_ID, ret);

    ret = SendMessage(sessionId, NULL, len);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);

    ret = SendMessage(sessionId, data, 0);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);
}


/**
 * @tc.name: TestSendStream001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestSendStream001, Function | MediumTest | Level0)
{
    int ret;
    int sessionId = 1;
    const StreamData streamData = {0};
    const StreamData ext = {0};
    const StreamFrameInfo param = {0};

    ret = SendStream(-1, &streamData, &ext, &param);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_TRANS_INVALID_SESSION_ID, ret);

    ret = SendStream(sessionId, NULL, &ext, &param);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);

    ret = SendStream(sessionId, &streamData, NULL, &param);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);

    ret = SendStream(sessionId, &streamData, &ext, NULL);
    TEST_ASSERT_EQUAL_INT(SOFTBUS_INVALID_PARAM, ret);
}

/**
 * @tc.name: TestQosReport00
 * @tc.desc: test the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestQosReport001, Function | MediumTest | Level0)
{
    int ret;
    ret = QosReport(1, 1, QOS_IMPROVE);
    TEST_ASSERT_TRUE(ret != SOFTBUS_OK);

    ret = QosReport(1, 1, QOS_RECOVER);
    TEST_ASSERT_TRUE(ret != SOFTBUS_OK);

    ret = QosReport(1, 1, QOS_IMPROVE);
    TEST_ASSERT_TRUE(ret != SOFTBUS_OK);
}

/**
 * @tc.name: TestSetFileSendListener001
 * @tc.desc: test the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestSetFileSendListener001, Function | MediumTest | Level0)
{
    int ret = SetFileSendListener(g_testModuleName, g_testSessionName, NULL);
    TEST_ASSERT_TRUE(ret == SOFTBUS_INVALID_PARAM);
}

/**
 * @tc.name: TestSendFile001
 * @tc.desc: test the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestSendFile001, Function | MediumTest | Level0)
{
    const char *sfileList[] = {
        "/data/big.tar",
        "/data/richu.jpg",
        "/data/richu-002.jpg",
        "/data/richu-003.jpg",
    };
    int ret = SendFile(g_sessionId, NULL, NULL, FILE_NUM);
    TEST_ASSERT_TRUE(ret == SOFTBUS_INVALID_PARAM);

    ret = SendFile(g_sessionId, sfileList, NULL, 0);
    TEST_ASSERT_TRUE(ret == SOFTBUS_INVALID_PARAM);

    ret = SendFile(g_sessionId, sfileList, NULL, FILE_NUM);
    TEST_ASSERT_TRUE(ret != SOFTBUS_OK);
}

/**
 * @tc.name: TestSetFileReceiveListener001
 * @tc.desc: test the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestSetFileReceiveListener001, Function | MediumTest | Level0)
{
    int ret = SetFileReceiveListener(g_testModuleName, g_testSessionNamE2, NULL, "/data/");
    TEST_ASSERT_TRUE(ret == SOFTBUS_INVALID_PARAM);
}

/**
 * @tc.name: TestGetMySessionName
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestGetMySessionName001, Function | MediumTest | Level0)
{
    int ret;
    int sessionId = -1;
    int len = 5;
    ret = GetMySessionName(sessionId, NULL, len);
    TEST_ASSERT_EQUAL_INT(ret, SOFTBUS_INVALID_PARAM);
}

/**
 * @tc.name: TestGetPeerSessionName001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestGetPeerSessionName001, Function | MediumTest | Level0)
{
    int ret;
    int sessionId = -1;
    int len = 5;
    ret = GetPeerSessionName(sessionId, NULL, len);
    TEST_ASSERT_EQUAL_INT(ret, SOFTBUS_INVALID_PARAM);
}

/**
 * @tc.name: TestGetPeerDeviceId001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestGetPeerDeviceId001, Function | MediumTest | Level0)
{
    int ret;
    int sessionId = -1;
    int len = 5;
    ret = GetPeerDeviceId(sessionId, NULL, len);
    TEST_ASSERT_EQUAL_INT(ret, SOFTBUS_INVALID_PARAM);
}

/**
 * @tc.name: TestGetSessionSide001
 * @tc.desc: extern module active publish, use the wrong parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
LITE_TEST_CASE(SessionTestSuite, TestGetSessionSide001, Function | MediumTest | Level0)
{
    int ret = GetSessionSide(g_sessionId);
    TEST_ASSERT_TRUE(ret != SOFTBUS_OK);
}

RUN_TEST_SUITE(SessionTestSuite);