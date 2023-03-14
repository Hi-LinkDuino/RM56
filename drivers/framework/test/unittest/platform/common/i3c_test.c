/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "i3c_test.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "i3c_if.h"
#include "osal_mem.h"
#include "osal_thread.h"
#include "osal_time.h"
#include "securec.h"

#define HDF_LOG_TAG i3c_test_c

#define I3C_TEST_MSG_NUM           2
#define I3C_TEST_8BIT              8
#define I3C_TEST_WAIT_TIMES        100
#define I3C_TEST_WAIT_TIMEOUT      20
#define I3C_TEST_STACK_SIZE        (1024 * 256)
#define I3C_TEST_IBI_PAYLOAD       16
#define I3C_TEST_REG_LEN           2

static struct I3cMsg g_msgs[I3C_TEST_MSG_NUM];
static uint8_t *g_buf;
static uint8_t g_regs[I3C_TEST_REG_LEN];

struct I3cTestEntry {
    int cmd;
    int32_t (*func)(void *param);
    const char *name;
};

static int32_t I3cTestGetTestConfig(struct I3cTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    service = HdfIoServiceBind("I3C_TEST");
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

struct I3cTester *I3cTesterGet(void)
{
    int32_t ret;
    static struct I3cTester tester;
    static bool hasInit = false;

    if (hasInit) {
        return &tester;
    }
    ret = I3cTestGetTestConfig(&tester.config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed, ret = %d", __func__, ret);
        return NULL;
    }
    tester.handle = I3cOpen(tester.config.busId);
    if (tester.handle == NULL) {
        HDF_LOGE("%s: Open I3C controller: %u failed!", __func__, tester.config.busId);
        return NULL;
    }
    hasInit = true;
    HDF_LOGD("%s: Done", __func__);
    return &tester;
}

static int32_t I3cTestMallocBuf(struct I3cTester *tester)
{
    struct I3cTestConfig *config = &tester->config;

    if (g_buf == NULL) {
        g_buf = OsalMemCalloc(config->bufSize);
        if (g_buf == NULL) {
            HDF_LOGE("I3cTestMallocBuf: malloc buf fail!");
            return HDF_ERR_MALLOC_FAIL;
        }
    }

    g_regs[0] = (uint8_t)config->regAddr;
    if (config->regLen > 1) {
        g_regs[1] = g_regs[0];
        g_regs[0] = (uint8_t)(config->regAddr >> I3C_TEST_8BIT);
    }

    g_msgs[0].addr = config->devAddr;
    g_msgs[0].flags = 0;
    g_msgs[0].len = config->regLen;
    g_msgs[0].buf = g_regs;

    g_msgs[1].addr = config->devAddr;
    g_msgs[1].flags = I3C_FLAG_READ;
    g_msgs[1].len = config->bufSize;
    g_msgs[1].buf = g_buf;

    return HDF_SUCCESS;
}

int32_t I3cTestSetUpAll(void *param)
{
    struct I3cTester *tester = NULL;
    struct I3cTestConfig *cfg = NULL;

    (void) param;
    HDF_LOGD("I3cTestSetUpAll: enter!");
    tester = I3cTesterGet();
    if (tester == NULL) {
        HDF_LOGE("I3cTestSetUpAll: get tester fail!");
        return HDF_ERR_INVALID_OBJECT;
    }
    tester->total = I3C_TEST_CMD_MAX;
    tester->fails = 0;

    cfg = &tester->config;
    HDF_LOGD("I3cTestSetUpAll: test on bus:0x%x, addr:0x%x, reg:0x%x, reglen:0x%x, size:0x%x",
        cfg->busId, cfg->devAddr, cfg->regAddr, cfg->regLen, cfg->bufSize);
    if (I3cTestMallocBuf(tester) != HDF_SUCCESS) {
        HDF_LOGE("I3cTestSetUpAll: set up test case fail!");
    }
    HDF_LOGD("I3cTestSetUpAll: exit!");
    *((int32_t *)param) = 1;

    return HDF_SUCCESS;
}

int32_t I3cTestTearDownAll(void *param)
{
    if (g_buf != NULL) {
        OsalMemFree(g_buf);
        g_buf = NULL;
    }
    *((int32_t *)param) = 1;

    return HDF_SUCCESS;
}

int32_t I3cTestSetUpSingle(void)
{
    return HDF_SUCCESS;
}

int32_t I3cTestTearDownSingle(void)
{
    return HDF_SUCCESS;
}

int32_t I3cTestTransfer(void *param)
{
    struct I3cTester *tester = NULL;
    int32_t ret;

    tester = I3cTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: Get tester failed!", __func__);
        *((int32_t *)param) = 1;
        return HDF_ERR_INVALID_OBJECT;
    }
    /* transfer one write msg */
    ret = I3cTransfer(tester->handle, g_msgs, 1, I3C_MODE);
    if (ret != 1) {
        HDF_LOGE("%s: I3cTransfer(write) err: %d", __func__, ret);
        *((int32_t *)param) = 1;
        return HDF_FAILURE;
    }

    /* transfer one read msg */
    ret = I3cTransfer(tester->handle, g_msgs + 1, 1, I3C_MODE);
    if (ret != 1) {
        HDF_LOGE("%s: I3cTransfer(read) err: %d", __func__, ret);
        *((int32_t *)param) = 1;
        return HDF_FAILURE;
    }

    /* transfer two msgs including a read msg and a write msg */
    ret = I3cTransfer(tester->handle, g_msgs, I3C_TEST_MSG_NUM, I3C_MODE);
    if (ret != I3C_TEST_MSG_NUM) {
        HDF_LOGE("%s: I3cTransfer(mix) err: %d", __func__, ret);
        *((int32_t *)param) = 1;
        return HDF_FAILURE;
    }
    *((int32_t *)param) = 1;
    HDF_LOGD("%s: Done", __func__);
    return HDF_SUCCESS;
}

int32_t I3cTestSetConfig(void *param)
{
    struct I3cTester *tester = NULL;
    struct I3cConfig *config = NULL;
    int32_t ret;

    tester = I3cTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: Get tester failed!", __func__);
        *((int32_t *)param) = 1;
        return HDF_ERR_INVALID_OBJECT;
    }

    config = (struct I3cConfig*)OsalMemCalloc(sizeof(*config));
    if (config == NULL) {
        HDF_LOGE("%s: MemCalloc failed!", __func__);
        *((int32_t *)param) = 1;
        return HDF_ERR_MALLOC_FAIL;
    }

    config->busMode = I3C_BUS_HDR_MODE;
    config->curMaster = NULL;
    ret = I3cSetConfig(tester->handle, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Set config failed!, busId = %d", __func__, tester->config.busId);
        OsalMemFree(config);
        *((int32_t *)param) = 1;
        return HDF_FAILURE;
    }
    OsalMemFree(config);
    config = NULL;
    HDF_LOGD("%s: Done", __func__);
    *((int32_t *)param) = 1;

    return HDF_SUCCESS;
}

int32_t I3cTestGetConfig(void *param)
{
    struct I3cTester *tester = NULL;
    struct I3cConfig *config = NULL;
    int32_t ret;

    tester = I3cTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: Get tester failed!", __func__);
        *((int32_t *)param) = 1;
        return HDF_ERR_INVALID_OBJECT;
    }

    config = (struct I3cConfig*)OsalMemCalloc(sizeof(*config));
    if (config == NULL) {
        HDF_LOGE("%s: MemCalloc failed!", __func__);
        *((int32_t *)param) = 1;
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = I3cGetConfig(tester->handle, config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Get config failed!, busId = %d", __func__, tester->config.busId);
        OsalMemFree(config);
        *((int32_t *)param) = 1;
        return HDF_FAILURE;
    }

    OsalMemFree(config);
    config = NULL;
    *((int32_t *)param) = 1;
    HDF_LOGD("%s: Done", __func__);

    return HDF_SUCCESS;
}

static int32_t TestI3cIbiFunc(DevHandle handle, uint16_t addr, struct I3cIbiData data)
{
    (void)handle;
    (void)addr;
    HDF_LOGD("%s: %.16s", __func__, (char *)data.buf);

    return HDF_SUCCESS;
}

int32_t I3cTestRequestIbi(void *param)
{
    struct I3cTester *tester = NULL;
    int32_t ret;

    tester = I3cTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: Get tester failed!", __func__);
        *((int32_t *)param) = 1;
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = I3cRequestIbi(tester->handle, tester->config.devAddr, TestI3cIbiFunc, I3C_TEST_IBI_PAYLOAD);
    if (ret != HDF_SUCCESS) {
        *((int32_t *)param) = 1;
        HDF_LOGE("%s: Requset IBI failed!, busId = %d", __func__, tester->config.busId);
        return HDF_FAILURE;
    }
    *((int32_t *)param) = 1;
    HDF_LOGD("%s: Done", __func__);

    return HDF_SUCCESS;
}

int32_t I3cTestFreeIbi(void *param)
{
    struct I3cTester *tester = NULL;
    int32_t ret;

    tester = I3cTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: Get tester failed!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = I3cFreeIbi(tester->handle, tester->config.devAddr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Free IBI failed!, busId = %d", __func__, tester->config.busId);
        *((int32_t *)param) = 1;
        return HDF_FAILURE;
    }
    *((int32_t *)param) = 1;
    HDF_LOGD("%s: done", __func__);

    return HDF_SUCCESS;
}

int32_t I3cTestThreadFunc(OsalThreadEntry func)
{
    int32_t ret;
    uint32_t time;
    struct OsalThread thread1, thread2;
    struct OsalThreadParam cfg1, cfg2;
    int32_t count1, count2;

    count1 = count2 = 0;
    time = 0;
    ret = OsalThreadCreate(&thread1, func, (void *)&count1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("create test thread1 fail:%d", ret);
        return HDF_FAILURE;
    }

    ret = OsalThreadCreate(&thread2, func, (void *)&count2);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("create test thread1 fail:%d", ret);
        return HDF_FAILURE;
    }

    cfg1.name = "I3cTestThread-1";
    cfg2.name = "I3cTestThread-2";
    cfg1.priority = cfg2.priority = OSAL_THREAD_PRI_DEFAULT;
    cfg1.stackSize = cfg2.stackSize = I3C_TEST_STACK_SIZE;

    ret = OsalThreadStart(&thread1, &cfg1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("start test thread1 fail:%d", ret);
        return HDF_FAILURE;
    }

    ret = OsalThreadStart(&thread2, &cfg2);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("start test thread2 fail:%d", ret);
        return HDF_FAILURE;
    }

    while (count1 == 0 || count2 == 0) {
        HDF_LOGE("waitting testing thread finish...");
        OsalMSleep(I3C_TEST_WAIT_TIMES);
        time++;
        if (time > I3C_TEST_WAIT_TIMEOUT) {
            break;
        }
    }

    (void)OsalThreadDestroy(&thread1);
    (void)OsalThreadDestroy(&thread2);
    HDF_LOGD("%s: done", __func__);

    return HDF_SUCCESS;
}

static struct I3cTestEntry g_multiThreadEntry[] = {
    { I3C_TEST_CMD_TRANSFER, I3cTestTransfer, "I3cTestTransfer" },
    { I3C_TEST_CMD_SET_CONFIG, I3cTestSetConfig, "I3cTestSetConfig" },
    { I3C_TEST_CMD_GET_CONFIG, I3cTestGetConfig, "I3cTestGetConfig" },
    { I3C_TEST_CMD_REQUEST_IBI, I3cTestRequestIbi, "I3cTestRequestIbi" },
    { I3C_TEST_CMD_FREE_IBI, I3cTestFreeIbi, "I3cTestFreeIbi" },
};

int32_t I3cTestMultiThread(void *param)
{
    uint32_t i;
    int32_t ret;

    for (i = 0; i < sizeof(g_multiThreadEntry) / sizeof(g_multiThreadEntry[0]); i++) {
        if (g_multiThreadEntry[i].func == NULL) {
            HDF_LOGE("%s: func is NULL", __func__);
            return HDF_FAILURE;
        }
        HDF_LOGI("%s: =================calling func %d =========================", __func__, i);
        ret = I3cTestThreadFunc((OsalThreadEntry)g_multiThreadEntry[i].func);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: Multithreading test failed: %u", __func__, i);
            return ret;
        }
    }
    *((int32_t *)param) = 1;

    return HDF_SUCCESS;
}

int32_t I3cTestReliability(void *param)
{
    struct I3cTester *tester = NULL;
    struct I3cConfig *config = NULL;

    tester = I3cTesterGet();
    if (tester == NULL || tester->handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    config = (struct I3cConfig *)OsalMemCalloc(sizeof(*config));
    if (config == NULL) {
        HDF_LOGE("func:%s config is NULL!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    config->busMode = I3C_BUS_HDR_MODE;
    config->curMaster = NULL;
    // invalid handle
    (void)I3cTransfer(NULL, g_msgs, 1, I3C_MODE);
    (void)I3cSetConfig(NULL, config);
    (void)I3cGetConfig(NULL, config);
    (void)I3cRequestIbi(NULL, tester->config.devAddr, TestI3cIbiFunc, I3C_TEST_IBI_PAYLOAD);
    (void)I3cFreeIbi(NULL, tester->config.devAddr);
    // Invalid msg
    (void)I3cTransfer(tester->handle, NULL, 1, I3C_MODE);
    // Invalid config
    (void)I3cSetConfig(tester->handle, NULL);
    (void)I3cGetConfig(tester->handle, NULL);
    // Invalid function
    (void)I3cRequestIbi(tester->handle, tester->config.devAddr, NULL, I3C_TEST_IBI_PAYLOAD);
    // Invalid number
    (void)I3cTransfer(tester->handle, g_msgs, -1, I3C_MODE);
    (void)I3cRequestIbi(tester->handle, tester->config.devAddr, TestI3cIbiFunc, -1);
    *((int32_t *)param) = 1;
    HDF_LOGD("%s: Done", __func__);

    return HDF_SUCCESS;
}

static struct I3cTestEntry g_entry[] = {
    { I3C_TEST_CMD_TRANSFER, I3cTestTransfer, "I3cTestTransfer" },
    { I3C_TEST_CMD_SET_CONFIG, I3cTestSetConfig, "I3cTestSetConfig" },
    { I3C_TEST_CMD_GET_CONFIG, I3cTestGetConfig, "I3cTestGetConfig" },
    { I3C_TEST_CMD_REQUEST_IBI, I3cTestRequestIbi, "I3cTestRequestIbi" },
    { I3C_TEST_CMD_FREE_IBI, I3cTestFreeIbi, "I3cTestFreeIbi" },
    { I3C_TEST_CMD_MULTI_THREAD, I3cTestMultiThread, "I3cTestMultiThread" },
    { I3C_TEST_CMD_RELIABILITY, I3cTestReliability, "I3cTestReliability" },
    { I3C_TEST_CMD_SETUP_ALL, I3cTestSetUpAll, "I3cTestSetUpAll" },
    { I3C_TEST_CMD_TEARDOWN_ALL, I3cTestTearDownAll, "I3cTestTearDownAll" },
};

int32_t I3cTestExecute(int cmd)
{
    uint32_t i;
    int32_t count;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

    if (cmd > I3C_TEST_CMD_MAX) {
        HDF_LOGE("%s: invalid cmd:%d", __func__, cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        HDF_LOGI("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
        return ret;
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        count = 0;
        ret = g_entry[i].func((void *)&count);
        break;
    }

    HDF_LOGI("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
    return ret;
}
