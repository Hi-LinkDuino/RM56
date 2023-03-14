/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "i2c_test.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "i2c_if.h"
#include "osal_mem.h"
#include "osal_thread.h"
#include "osal_time.h"
#include "securec.h"

#define HDF_LOG_TAG i2c_test

#define I2C_TEST_8BIT          8
#define I2C_TEST_MSG_NUM       2
#define I2C_TEST_BUF_SIZE_MAX  128
#define I2C_TEST_REG_LEN       2
#define I2C_TEST_MLTTHD_TIMES  1000
#define I2C_TEST_STACK_SIZE    (1024 * 100)
#define I2C_TEST_WAIT_TIMES    200

static struct I2cMsg g_msgs[I2C_TEST_MSG_NUM];
static uint8_t *g_buf;
static uint8_t g_regs[I2C_TEST_REG_LEN];

static int32_t I2cTestGetConfig(struct I2cTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    HDF_LOGD("I2cTestGetConfig: enter!");
    service = HdfIoServiceBind("I2C_TEST");
    if (service == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    reply = HdfSbufObtain(sizeof(*config) + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("I2cTestGetConfig: failed to obtain reply!");
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("I2cTestGetConfig: remote dispatch fail:%d", ret);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    if (!HdfSbufReadBuffer(reply, &buf, &len)) {
        HDF_LOGE("I2cTestGetConfig: read buf fail!");
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    if (len != sizeof(*config)) {
        HDF_LOGE("I2cTestGetConfig: config size:%u, but read size:%u!", sizeof(*config), len);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    if (memcpy_s(config, sizeof(*config), buf, sizeof(*config)) != EOK) {
        HDF_LOGE("I2cTestGetConfig: memcpy buf fail!");
        ret = HDF_ERR_IO;
        goto EXIT;
    }
    ret = HDF_SUCCESS;
EXIT:
    HdfSbufRecycle(reply);
    HDF_LOGD("I2cTestGetConfig: exit!");
    HdfIoServiceRecycle(service);
    return ret;
}

struct I2cTester *I2cTesterGet(void)
{
    int32_t ret;
    static struct I2cTester tester;
    static bool hasInit = false;

    if (hasInit) {
        return &tester;
    }

    ret = I2cTestGetConfig(&tester.config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("I2cTesterGet: read config fail:%d", ret);
        return NULL;
    }

    tester.handle = I2cOpen(tester.config.busNum);
    if (tester.handle == NULL) {
        HDF_LOGE("I2cTesterGet: open i2cBus:%u fail!", tester.config.busNum);
        return NULL;
    }

    hasInit = true;
    return &tester;
}

static int32_t I2cTestMallocBuf(struct I2cTester *tester)
{
    struct I2cTestConfig *config = &tester->config;

    if (g_buf == NULL) {
        g_buf = OsalMemCalloc(config->bufSize);
        if (g_buf == NULL) {
            HDF_LOGE("I2cTestMallocBuf: malloc buf fail!");
            return HDF_ERR_MALLOC_FAIL;
        }
    }

    g_regs[0] = (uint8_t)config->regAddr;
    if (config->regLen > 1) {
        g_regs[1] = g_regs[0];
        g_regs[0] = (uint8_t)(config->regAddr >> I2C_TEST_8BIT);
    }

    g_msgs[0].addr = config->devAddr;
    g_msgs[0].flags = 0;
    g_msgs[0].len = config->regLen;
    g_msgs[0].buf = g_regs;

    g_msgs[1].addr = config->devAddr;
    g_msgs[1].flags = I2C_FLAG_READ;
    g_msgs[1].len = config->bufSize;
    g_msgs[1].buf = g_buf;

    return HDF_SUCCESS;
}

int32_t I2cTestSetUpAll(void)
{
    struct I2cTester *tester = NULL;
    struct I2cTestConfig *cfg = NULL;

    HDF_LOGD("I2cTestSetUpAll: enter!");
    tester = I2cTesterGet();
    if (tester == NULL) {
        HDF_LOGE("I2cTestSetUpAll: get tester fail!");
        return HDF_ERR_INVALID_OBJECT;
    }
    tester->total = I2C_TEST_CMD_MAX;
    tester->fails = 0;

    cfg = &tester->config;
    HDF_LOGE("I2cTestSetUpAll: test on bus:0x%x, addr:0x%x, reg:0x%x, reglen:0x%x, size:0x%x",
        cfg->busNum, cfg->devAddr, cfg->regAddr, cfg->regLen, cfg->bufSize);
    if (I2cTestMallocBuf(tester) != HDF_SUCCESS) {
        HDF_LOGE("I2cTestSetUpAll: set up test case fail!");
    }
    HDF_LOGD("I2cTestSetUpAll: exit!");
    return HDF_SUCCESS;
}

int32_t I2cTestTearDownAll(void)
{
    if (g_buf != NULL) {
        OsalMemFree(g_buf);
        g_buf = NULL;
    }

    return HDF_SUCCESS;
}

int32_t I2cTestSetUpSingle(void)
{
    return HDF_SUCCESS;
}

int32_t I2cTestTearDownSingle(void)
{
    return HDF_SUCCESS;
}

int32_t I2cTestTransfer(void)
{
    int32_t ret;
    struct I2cTester *tester = NULL;

    tester = I2cTesterGet();
    if (tester == NULL || tester->handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /* transfer one write msg */
    ret = I2cTransfer(tester->handle, g_msgs, 1);
    if (ret != 1) {
        HDF_LOGE("I2cTestTransfer: I2cTransfer(write) err:%d", ret);
        return HDF_FAILURE;
    }

    /* transfer one read msg */
    ret = I2cTransfer(tester->handle, g_msgs + 1, 1);
    if (ret != 1) {
        HDF_LOGE("I2cTestTransfer: I2cTransfer(read) err:%d", ret);
        return HDF_FAILURE;
    }

    /* transfer two msgs including a read msg and a write msg */
    ret = I2cTransfer(tester->handle, g_msgs, I2C_TEST_MSG_NUM);
    if (ret != I2C_TEST_MSG_NUM) {
        HDF_LOGE("I2cTestTransfer: I2cTransfer(mix) err:%d", ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t I2cTestWriteRead(void)
{
    return HDF_SUCCESS;
}

static int I2cTestThreadFunc(void *param)
{
    int32_t i, ret;
    struct I2cTester *tester = NULL;

    tester = I2cTesterGet();
    if (tester == NULL || tester->handle == NULL) {
        *((int32_t *)param) = 1;
        return HDF_ERR_INVALID_OBJECT;
    }

    for (i = 0; i < I2C_TEST_MLTTHD_TIMES; i++) {
        ret = I2cTransfer(tester->handle, g_msgs, I2C_TEST_MSG_NUM);
        if (ret != I2C_TEST_MSG_NUM) {
#ifdef __USER__
            HDF_LOGE("I2cTransfer-user err in multithread test:%d", ret);
#else
            HDF_LOGE("I2cTransfer-kerl err in multithread test:%d", ret);
#endif
        }
    }

    *((int32_t *)param) = 1;
    return HDF_SUCCESS;
}

int32_t I2cTestMultiThread(void)
{
    int32_t ret;
    struct OsalThread thread1, thread2;
    struct OsalThreadParam cfg1, cfg2;
    int32_t count1, count2;

    count1 = count2 = 0;

    ret = OsalThreadCreate(&thread1, (OsalThreadEntry)I2cTestThreadFunc, (void *)&count1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("create test thread1 fail:%d", ret);
        return HDF_FAILURE;
    }

    ret = OsalThreadCreate(&thread2, (OsalThreadEntry)I2cTestThreadFunc, (void *)&count2);
    if (ret != HDF_SUCCESS) {
        (void)OsalThreadDestroy(&thread1);
        HDF_LOGE("create test thread1 fail:%d", ret);
        return HDF_FAILURE;
    }

    cfg1.name = "I2cTestThread-1";
    cfg2.name = "I2cTestThread-2";
    cfg1.priority = cfg2.priority = OSAL_THREAD_PRI_DEFAULT;
    cfg1.stackSize = cfg2.stackSize = I2C_TEST_STACK_SIZE;

    ret = OsalThreadStart(&thread1, &cfg1);
    if (ret != HDF_SUCCESS) {
        (void)OsalThreadDestroy(&thread1);
        (void)OsalThreadDestroy(&thread2);
        HDF_LOGE("start test thread1 fail:%d", ret);
        return HDF_FAILURE;
    }

    ret = OsalThreadStart(&thread2, &cfg2);
    if (ret != HDF_SUCCESS) {
        (void)OsalThreadDestroy(&thread1);
        (void)OsalThreadDestroy(&thread2);
        HDF_LOGE("start test thread2 fail:%d", ret);
        return HDF_FAILURE;
    }

    while (count1 == 0 || count2 == 0) {
        HDF_LOGE("waitting testing thread finish...");
        OsalMSleep(I2C_TEST_WAIT_TIMES);
    }

    (void)OsalThreadDestroy(&thread1);
    (void)OsalThreadDestroy(&thread2);
    return HDF_SUCCESS;
}

int32_t I2cTestReliability(void)
{
    struct I2cTester *tester = NULL;

    HDF_LOGD("I2cTestReliability: test dfr for I2cTransfer ...");
    tester = I2cTesterGet();
    if (tester == NULL || tester->handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    /* invalid device handle */
    (void)I2cTransfer(NULL, g_msgs, I2C_TEST_MSG_NUM);
    /* invalid device msgs */
    (void)I2cTransfer(tester->handle, NULL, I2C_TEST_MSG_NUM);
    /* invalid device msg number */
    (void)I2cTransfer(tester->handle, g_msgs, -1);

    return HDF_SUCCESS;
}

int32_t I2cTestPeformance(void)
{
#ifdef __LITEOS__
    // liteos the accuracy of the obtained time is too large and inaccurate.
    return HDF_SUCCESS;
#endif
    uint64_t startMs;
    uint64_t endMs;
    uint64_t useTime;    // ms
    struct I2cTester *tester = NULL;
    DevHandle handle = NULL;

    tester = I2cTesterGet();
    if (tester == NULL || tester->handle == NULL) {
        HDF_LOGE("%s:get tester fail", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    startMs = OsalGetSysTimeMs();
    handle = I2cOpen(tester->config.busNum);
    endMs = OsalGetSysTimeMs();

    if (handle != NULL) {
        useTime = endMs - startMs;
        HDF_LOGI("----->interface performance test:[start:%lld(ms) - end:%lld(ms) = %lld (ms)] < 1ms[%d]\r\n",
            startMs, endMs, useTime, useTime < 1 ? true : false);
        I2cClose(handle);
        return HDF_SUCCESS;
    }

    return HDF_FAILURE;
}

struct I2cTestEntry {
    int cmd;
    int32_t (*func)(void);
    const char *name;
};

static struct I2cTestEntry g_entry[] = {
    { I2C_TEST_CMD_TRANSFER, I2cTestTransfer, "I2cTestTransfer" },
    { I2C_TEST_CMD_WRITE_READ, I2cTestWriteRead, "I2cTestWriteRead" },
    { I2C_TEST_CMD_MULTI_THREAD, I2cTestMultiThread, "I2cTestMultiThread" },
    { I2C_TEST_CMD_RELIABILITY, I2cTestReliability, "I2cTestReliability" },
    { I2C_TEST_CMD_PERFORMANCE, I2cTestPeformance, "I2cTestPeformance" },
    { I2C_TEST_CMD_SETUP_ALL, I2cTestSetUpAll, "I2cTestSetUpAll" },
    { I2C_TEST_CMD_TEARDOWN_ALL, I2cTestTearDownAll, "I2cTestTearDownAll" },
    { I2C_TEST_CMD_SETUP_SINGLE, I2cTestSetUpSingle, "I2cTestSetUpSingle" },
    { I2C_TEST_CMD_TEARDOWN_SINGLE, I2cTestTearDownSingle, "I2cTestTearDownSingle" },
};

int32_t I2cTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

    if (cmd > I2C_TEST_CMD_MAX) {
        HDF_LOGE("I2cTestExecute: invalid cmd:%d", cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        HDF_LOGE("[I2cTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
        return ret;
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        ret = g_entry[i].func();
        break;
    }

    HDF_LOGE("[I2cTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
    return ret;
}

void I2cTestExecuteAll(void)
{
    int32_t i;
    int32_t ret;
    int32_t fails = 0;

    /* setup env for all test cases */
    (void)I2cTestExecute(I2C_TEST_CMD_SETUP_ALL);

    for (i = 0; i <= I2C_TEST_CMD_RELIABILITY; i++) {
        ret = I2cTestExecute(i);
        fails += (ret != HDF_SUCCESS) ? 1 : 0;
    }

    /* teardown env for all test cases */
    (void)I2cTestExecute(I2C_TEST_CMD_TEARDOWN_ALL);

    HDF_LOGE("I2cTestExecuteALL: **********PASS:%d  FAIL:%d************\n\n",
        I2C_TEST_CMD_RELIABILITY + 1 - fails, fails);
}
