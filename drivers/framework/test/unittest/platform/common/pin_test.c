/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pin_test.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "pin_if.h"
#include "securec.h"

#define HDF_LOG_TAG pin_test_c

#define PIN_FUNC_NAME_LENGTH       30

struct PinCfgs {
    enum PinPullType pullTypeNum;
    uint32_t strengthNum;
    const char *funcName;
} g_oldPinCfg;

struct PinTestEntry {
    int cmd;
    int32_t (*func)(void);
    const char *name;
};

static int32_t PinTestGetTestConfig(struct PinTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    HDF_LOGD("%s: enter", __func__);
    service = HdfIoServiceBind("PIN_TEST");
    if (service == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    reply = HdfSbufObtain(sizeof(*config) + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("%s: Failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Remote dispatch failed", __func__);
        return ret;
    }

    if (!HdfSbufReadBuffer(reply, &buf, &len)) {
        HDF_LOGE("%s: Read buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (len != sizeof(*config)) {
        HDF_LOGE("%s: Config size:%zu, read size:%u", __func__, sizeof(*config), len);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (memcpy_s(config, sizeof(*config), buf, sizeof(*config)) != EOK) {
        HDF_LOGE("%s: Memcpy buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }
    HdfSbufRecycle(reply);
    HDF_LOGD("%s: Done", __func__);
    return HDF_SUCCESS;
}

struct PinTester *PinTesterGet(void)
{
    int32_t ret;
    static struct PinTester tester;
    static bool hasInit = false;

    if (hasInit) {
        return &tester;
    }
    HDF_LOGI("%s: enter", __func__);
    if (hasInit) {
        return &tester;
    }
    ret = PinTestGetTestConfig(&tester.config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed:%d", __func__, ret);
        return NULL;
    }
    tester.handle = PinGet(tester.config.pinName);
    if (tester.handle == NULL) {
        HDF_LOGE("%s: open pin:%s failed", __func__, tester.config.pinName);
        return NULL;
    }
    hasInit = true;
    HDF_LOGD("%s: Done", __func__);
    return &tester;
}

static int32_t PinSetGetPullTest(void)
{
    struct PinTester *tester = NULL;
    int32_t ret;
    enum PinPullType getPullTypeNum;

    getPullTypeNum = 0;
    tester = PinTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: Get tester failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = PinSetPull(tester->handle, tester->config.PullTypeNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pin set pull failed!, pinName:%s", __func__, tester->config.pinName);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: Pin set pull success!, PullTypeNum:%d", __func__, tester->config.PullTypeNum);
    ret = PinGetPull(tester->handle, &getPullTypeNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pin get pull failed!, pinName:%s", __func__, tester->config.pinName);
        return HDF_FAILURE;
    }
    if (tester->config.PullTypeNum != getPullTypeNum) {
        HDF_LOGE("%s: Pin set pull:%d, but Pin get pull:%u", __func__, tester->config.PullTypeNum, getPullTypeNum);
    }
    HDF_LOGD("%s: done", __func__);
    return HDF_SUCCESS;
}

static int32_t PinSetGetStrengthTest(void)
{
    struct PinTester *tester = NULL;
    int32_t ret;
    uint32_t getStrengthNum;

    getStrengthNum = 0;
    tester = PinTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: Get tester failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = PinSetStrength(tester->handle, tester->config.strengthNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pin set strength failed!, pinName:%s", __func__, tester->config.pinName);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: Pin set strength success!,strengthNum:%d", __func__, tester->config.strengthNum);
    ret = PinGetStrength(tester->handle, &getStrengthNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pin get pull failed!, pinName:%s", __func__, tester->config.pinName);
        return HDF_FAILURE;
    }
    if (tester->config.strengthNum != getStrengthNum) {
        HDF_LOGE("%s: Pin set strength:%d, but Pin get strength:%d", __func__,
            tester->config.strengthNum, getStrengthNum);
    }
    HDF_LOGD("%s: done", __func__);
    return HDF_SUCCESS;
}

static int32_t PinSetGetFuncTest(void)
{
    struct PinTester *tester = NULL;
    int32_t ret;

    tester = PinTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: Get tester failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = PinSetFunc(tester->handle, (const char *)tester->config.funcNameBuf);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pin set function failed!, pinName:%s, functionName:%s", __func__,
            tester->config.pinName, tester->config.funcNameBuf);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: Pin set function success!, pinName:%s, functionName:%s", __func__,
        tester->config.pinName, tester->config.funcNameBuf);
    ret = PinGetFunc(tester->handle, &tester->config.funcName);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pin get function failed!, pinName:%s, functionName:%s", __func__,
            tester->config.pinName, tester->config.funcName);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: Pin get function success!, pinName:%s, functionName:%s", __func__,
        tester->config.pinName, tester->config.funcName);
    if (strcmp((const char *)tester->config.funcNameBuf, tester->config.funcName) != 0) {
        HDF_LOGE("%s: Pin set function:%s, but Pin get function:%s", __func__,
            tester->config.funcNameBuf, tester->config.funcName);
    }
    HDF_LOGD("%s: done", __func__);
    return HDF_SUCCESS;
}

int32_t PinTestSetUpAll(void)
{
    int32_t ret;
    struct PinTester *tester = NULL;
    struct PinTestConfig *cfg = NULL;

    HDF_LOGD("%s: enter!", __func__);
    tester = PinTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    tester->total = PIN_TEST_CMD_MAX;
    tester->fails = 0;

    cfg = &tester->config;
    HDF_LOGD("%s: test on pinName:%s, PullTypeNum:%d, strengthNum:%d", __func__,
        cfg->pinName, cfg->PullTypeNum, cfg->strengthNum);
    ret = PinGetPull(tester->handle, &g_oldPinCfg.pullTypeNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get pullTypeNum failed!", __func__);
        return ret;
    }
    ret = PinGetStrength(tester->handle, &g_oldPinCfg.strengthNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get strengthNum failed!", __func__);
        return ret;
    }
    g_oldPinCfg.funcName = (char *)OsalMemCalloc(PIN_FUNC_NAME_LENGTH * sizeof(char));
    if (g_oldPinCfg.funcName == NULL) {
        HDF_LOGE("%s: alloc g_oldPinCfg.funcName failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    tester->config.funcName = (char *)OsalMemCalloc(PIN_FUNC_NAME_LENGTH * sizeof(char));
    if (tester->config.funcName == NULL) {
        HDF_LOGE("%s: alloc tester->config.funcName failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = PinGetFunc(tester->handle, &g_oldPinCfg.funcName);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get funcName failed!", __func__);
        return ret;
    }
    HDF_LOGI(":%s old funcName:%s", __func__, g_oldPinCfg.funcName);
    HDF_LOGD("%s: exit!", __func__);

    return HDF_SUCCESS;
}

int32_t PinTestTearDownAll(void)
{
    int32_t ret;
    struct PinTester *tester = NULL;

    HDF_LOGD("%s: enter!", __func__);
    tester = PinTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = PinSetPull(tester->handle, g_oldPinCfg.pullTypeNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set pullTypeNum failed!", __func__);
        return ret;
    }
    ret = PinSetStrength(tester->handle, g_oldPinCfg.strengthNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set strengthNum failed!", __func__);
        return ret;
    }
    ret = PinSetFunc(tester->handle, g_oldPinCfg.funcName);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set funcName failed!", __func__);
        return ret;
    }
    HDF_LOGD("%s: exit!", __func__);

    return HDF_SUCCESS;
}

int32_t PinTestSetUpSingle(void)
{
    return HDF_SUCCESS;
}

int32_t PinTestTearDownSingle(void)
{
    return HDF_SUCCESS;
}

int32_t PinTestReliability(void)
{
    struct PinTester *tester = NULL;

    HDF_LOGI("%s: enter", __func__);
    tester = PinTesterGet();
    if (tester == NULL || tester->handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    HDF_LOGD("%s: test dfr for PinSetPull ...", __func__);
    // invalid handle
    (void)PinSetPull(NULL, tester->config.PullTypeNum);
    (void)PinGetPull(NULL, &tester->config.PullTypeNum);
    (void)PinSetStrength(NULL, tester->config.strengthNum);
    (void)PinGetStrength(NULL, &tester->config.strengthNum);
    (void)PinSetFunc(NULL, (const char *)tester->config.funcNameBuf);
    (void)PinGetFunc(NULL, &tester->config.funcName);
    // invalid strengthNum
    (void)PinSetStrength(tester->handle, -1);
    (void)PinGetStrength(tester->handle, NULL);
    // invalid FuncName
    (void)PinSetFunc(tester->handle, NULL);
    HDF_LOGD("%s:test done.All completed!", __func__);

    return HDF_SUCCESS;
}

static struct PinTestEntry g_entry[] = {
    { PIN_TEST_CMD_SETGETPULL, PinSetGetPullTest, "PinSetGetPullTest" },
    { PIN_TEST_CMD_SETGETSTRENGTH, PinSetGetStrengthTest, "PinSetGetStrengthTest" },
    { PIN_TEST_CMD_SETGETFUNC, PinSetGetFuncTest, "PinSetGetFuncTest" },
    { PIN_TEST_CMD_RELIABILITY, PinTestReliability, "PinTestReliability" },
    { PIN_TEST_CMD_SETUP_ALL, PinTestSetUpAll, "PinTestSetUpAll" },
    { PIN_TEST_CMD_TEARDOWN_ALL, PinTestTearDownAll, "PinTestTearDownAll" },
};

int32_t PinTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

    if (cmd > PIN_TEST_CMD_MAX) {
        HDF_LOGE("%s: invalid cmd:%d", __func__, cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        HDF_LOGE("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
        return ret;
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        ret = g_entry[i].func();
        break;
    }

    HDF_LOGE("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
    return ret;
}

void PinTestExecuteAll(void)
{
    int32_t i;
    int32_t ret;
    int32_t fails = 0;

    /* setup env for all test cases */
    ret = PinTestExecute(PIN_TEST_CMD_SETUP_ALL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("func:%s PinTestExecute SETUP failed", __func__);
    }

    for (i = 0; i < PIN_TEST_CMD_SETUP_ALL; i++) {
        ret = PinTestExecute(i);
        fails += (ret != HDF_SUCCESS) ? 1 : 0;
    }

    /* teardown env for all test cases */
    ret = PinTestExecute(PIN_TEST_CMD_TEARDOWN_ALL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("func:%s PinTestExecute TEARDOWN failed", __func__);
    }

    HDF_LOGI("%s: **********PASS:%d  FAIL:%d************\n\n",
        __func__, PIN_TEST_CMD_RELIABILITY + 1 - fails, fails);
}
