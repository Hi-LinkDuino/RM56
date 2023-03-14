/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "adc_test.h"
#include "adc_if.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "osal_thread.h"
#include "osal_time.h"


#define HDF_LOG_TAG adc_test_c

#define TEST_ADC_VAL_NUM           50
#define ADC_TEST_WAIT_TIMES      100
#define ADC_TEST_STACK_SIZE        (1024 * 64)

static int32_t AdcTestGetConfig(struct AdcTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    HDF_LOGD("%s: enter", __func__);
    service = HdfIoServiceBind("ADC_TEST");
    if (service == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    reply = HdfSbufObtain(sizeof(*config) + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: remote dispatch failed", __func__);
        return ret;
    }

    if (!HdfSbufReadBuffer(reply, &buf, &len)) {
        HDF_LOGE("%s: read buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (len != sizeof(*config)) {
        HDF_LOGE("%s: config size:%zu, read size:%u", __func__, sizeof(*config), len);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (memcpy_s(config, sizeof(*config), buf, sizeof(*config)) != EOK) {
        HDF_LOGE("%s: memcpy buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }
    HdfSbufRecycle(reply);
    HDF_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

struct AdcTester *AdcTesterGet(void)
{
    int32_t ret;
    static struct AdcTester tester;
    static bool hasInit = false;

    HDF_LOGE("%s: enter", __func__);
    if (hasInit) {
        return &tester;
    }
    ret = AdcTestGetConfig(&tester.config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed:%d", __func__, ret);
        return NULL;
    }
    tester.handle = AdcOpen(tester.config.devNum);
    if (tester.handle == NULL) {
        HDF_LOGE("%s: open adc device:%u failed", __func__, tester.config.devNum);
        return NULL;
    }
    hasInit = true;
    HDF_LOGI("%s: done", __func__);
    return &tester;
}

int32_t AdcTestRead(void)
{
    struct AdcTester *tester = NULL;
    uint32_t value[TEST_ADC_VAL_NUM];
    int32_t ret;
    int i;

    HDF_LOGI("%s: enter", __func__);
    tester = AdcTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    for (i = 0; i < TEST_ADC_VAL_NUM; i++) {
        value[i] = 0;
        ret = AdcRead(tester->handle, tester->config.channel, &value[i]);
        if (ret != HDF_SUCCESS || value[i] >= (1U << tester->config.dataWidth)) {
            HDF_LOGE("%s: read value failed, ret:%d", __func__, ret);
            return HDF_ERR_IO;
        }
    }

    HDF_LOGI("%s: done", __func__);
    return HDF_SUCCESS;
}

static int AdcTestThreadFunc(void *param)
{
    struct AdcTester *tester = NULL;
    uint32_t val;
    int i;
    int32_t ret;

    HDF_LOGI("%s: enter", __func__);
    tester = AdcTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        *((int32_t *)param) = 1;
        return HDF_ERR_INVALID_OBJECT;
    }

    for (i = 0; i < ADC_TEST_WAIT_TIMES; i++) {
        ret = AdcRead(tester->handle, tester->config.channel, &val);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: AdcRead failed, ret:%d", __func__, ret);
            *((int32_t *)param) = 1;
            return HDF_ERR_IO;
        }
    }

    *((int32_t *)param) = 1;
    HDF_LOGI("%s: done", __func__);
    return val;
}

int32_t AdcTestMultiThread(void)
{
    int32_t ret;
    struct OsalThread thread1, thread2;
    struct OsalThreadParam cfg1, cfg2;
    int32_t count1 = 0;
    int32_t count2 = 0;

    HDF_LOGI("%s: enter", __func__);
    ret = OsalThreadCreate(&thread1, (OsalThreadEntry)AdcTestThreadFunc, (void *)&count1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("create test thread1 fail:%d", ret);
        return HDF_FAILURE;
    }

    ret = OsalThreadCreate(&thread2, (OsalThreadEntry)AdcTestThreadFunc, (void *)&count2);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("create test thread1 fail:%d", ret);
        return HDF_FAILURE;
    }

    cfg1.name = "AdcTestThread-1";
    cfg2.name = "AdcTestThread-2";
    cfg1.priority = cfg2.priority = OSAL_THREAD_PRI_DEFAULT;
    cfg1.stackSize = cfg2.stackSize = ADC_TEST_STACK_SIZE;

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
        OsalMSleep(ADC_TEST_WAIT_TIMES);
    }

    (void)OsalThreadDestroy(&thread1);
    (void)OsalThreadDestroy(&thread2);
    HDF_LOGI("%s: done", __func__);
    return HDF_SUCCESS;
}

int32_t AdcTestReliability(void)
{
    struct AdcTester *tester = NULL;
    uint32_t val;

    HDF_LOGI("%s: enter", __func__);
    tester = AdcTesterGet();
    if (tester == NULL || tester->handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    HDF_LOGD("%s: test dfr for AdcRead ...", __func__);
    // invalid handle
    (void)AdcRead(NULL, tester->config.channel, &val);
    // invalid channel
    (void)AdcRead(tester->handle, tester->config.maxChannel + 1, &val);
    // invalid val pointer
    (void)AdcRead(tester->handle, tester->config.channel, NULL);
    HDF_LOGI("%s: done", __func__);
    return HDF_SUCCESS;
}

static int32_t AdcIfPerformanceTest(void)
{
#ifdef __LITEOS__
    // liteos the accuracy of the obtained time is too large and inaccurate.
    return HDF_SUCCESS;
#endif
    struct AdcTester *tester = NULL;
    uint64_t startMs;
    uint64_t endMs;
    uint64_t useTime;    // ms
    uint32_t val;
    int32_t ret;

    tester = AdcTesterGet();
    if (tester == NULL || tester->handle == NULL) {
        HDF_LOGE("%s:get tester fail", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    startMs = OsalGetSysTimeMs();
    ret = AdcRead(tester->handle, tester->config.channel, &val);
    if (ret == HDF_SUCCESS) {
        endMs = OsalGetSysTimeMs();
        useTime = endMs - startMs;
        HDF_LOGI("----->interface performance test:[start:%lld(ms) - end:%lld(ms) = %lld (ms)] < 1ms[%d]\r\n",
            startMs, endMs, useTime, useTime < 1 ? true : false);
    }
    return HDF_FAILURE;
}

struct AdcTestEntry {
    int cmd;
    int32_t (*func)(void);
    const char *name;
};

static struct AdcTestEntry g_entry[] = {
    { ADC_TEST_CMD_READ, AdcTestRead, "AdcTestRead" },
    { ADC_TEST_CMD_MULTI_THREAD, AdcTestMultiThread, "AdcTestMultiThread" },
    { ADC_TEST_CMD_RELIABILITY, AdcTestReliability, "AdcTestReliability" },
    { ADC_IF_PERFORMANCE_TEST, AdcIfPerformanceTest, "AdcIfPerformanceTest" },
};

int32_t AdcTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

    if (cmd > ADC_TEST_CMD_MAX) {
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
