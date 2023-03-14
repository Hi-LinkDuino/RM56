/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "timer_test.h"
#include "device_resource_if.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_thread.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "timer_if.h"

#define HDF_LOG_TAG timer_test_c

static bool g_theard1Flag = false;
static bool g_theard2Flag = false;

struct TimerTestFunc {
    int type;
    int32_t (*Func)(struct TimerTest *test);
};

int32_t TimerTestcaseCb(void)
{
    static uint16_t num = 0;
    num++;
    if (num >= TIMER_TEST_PERIOD_TIMES) {
        HDF_LOGD("->>>>>>>>>>>%s:num exceed max", __func__);
        g_theard2Flag = true;
    }
    return HDF_SUCCESS;
}

int32_t TimerTestcaseOnceCb(void)
{
    HDF_LOGD("->>>>>>>>>>>%s:", __func__);
    g_theard1Flag = true;
    return HDF_SUCCESS;
}

static int32_t TimerSetTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HwTimerSet(test->handle, test->uSecond, TimerTestcaseCb);
    return HDF_SUCCESS;
}

static int32_t TimerSetOnceTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HwTimerSetOnce(test->handle, test->uSecond, TimerTestcaseOnceCb);
    return HDF_SUCCESS;
}

static int32_t TimerGetTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    uint32_t uSecond;
    bool isPeriod;

    if (HwTimerGet(test->handle, &uSecond, &isPeriod) != HDF_SUCCESS) {
        HDF_LOGE("func: %s, TimerGet dailed", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGD("%s:[%u][%d]", __func__, uSecond, isPeriod);
    return HDF_SUCCESS;
}

static int32_t TimerStartTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HwTimerStart(test->handle);
    return HDF_SUCCESS;
}

static int32_t TimerStopTest(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HwTimerStop(test->handle);
    return HDF_SUCCESS;
}

static int TimerOnceTestThreadFunc(void *param)
{
    DevHandle handle = (DevHandle)param;
    if (handle == NULL) {
        HDF_LOGE("%s: timer test get handle fail", __func__);
        g_theard1Flag = true;
        return HDF_FAILURE;
    }

    if (HwTimerSetOnce(handle, TIMER_TEST_TIME_USECONDS, TimerTestcaseOnceCb) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerSetOnce fail", __func__);
        g_theard1Flag = true;
        return HDF_FAILURE;
    }
    if (HwTimerStart(handle) != HDF_SUCCESS) {
        HDF_LOGE("%s: HwTimerStart fail", __func__);
        g_theard1Flag = true;
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int TimerPeriodTestThreadFunc(void *param)
{
    DevHandle handle = (DevHandle)param;
    if (handle == NULL) {
        HDF_LOGE("%s: timer test get handle fail", __func__);
        g_theard2Flag = true;
        return HDF_FAILURE;
    }

    if (HwTimerSet(handle, TIMER_TEST_TIME_USECONDS, TimerTestcaseCb) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerSet fail", __func__);
        g_theard2Flag = true;
        return HDF_FAILURE;
    }
    if (HwTimerStart(handle) != HDF_SUCCESS) {
        HDF_LOGE("%s: HwTimerStart fail", __func__);
        g_theard2Flag = true;
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t TimerTestMultiThread(struct TimerTest *test)
{
    int32_t ret;
    uint32_t time = 0;
    struct OsalThread thread1, thread2;
    struct OsalThreadParam cfg1, cfg2;
    DevHandle handle1 = NULL;
    DevHandle handle2 = NULL;
    if (test == NULL) {
        HDF_LOGE("%s: timer test NULL", __func__);
        return HDF_FAILURE;
    }
    thread1.realThread = NULL;
    thread2.realThread = NULL;

    do {
        handle1 = HwTimerOpen(TIMER_TEST_TIME_ID_THREAD1);
        if (handle1 == NULL) {
            HDF_LOGE("%s: timer test get handle1 fail", __func__);
            ret = HDF_FAILURE;
            break;
        }
        handle2 = HwTimerOpen(TIMER_TEST_TIME_ID_THREAD2);
        if (handle1 == NULL) {
            HDF_LOGE("%s: timer test get handle2 fail", __func__);
            ret = HDF_FAILURE;
            break;
        }

        ret = OsalThreadCreate(&thread1, (OsalThreadEntry)TimerOnceTestThreadFunc, (void *)handle1);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("create test once timer fail:%d", ret);
            ret = HDF_FAILURE;
            break;
        }

        ret = OsalThreadCreate(&thread2, (OsalThreadEntry)TimerPeriodTestThreadFunc, (void *)handle2);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("create test period timer fail:%d", ret);
            ret = HDF_FAILURE;
            break;
        }

        cfg1.name = "TimerTestThread-once";
        cfg2.name = "TimerTestThread-period";
        cfg1.priority = cfg2.priority = OSAL_THREAD_PRI_DEFAULT;
        cfg1.stackSize = cfg2.stackSize = TIMER_TEST_STACK_SIZE;

        ret = OsalThreadStart(&thread1, &cfg1);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("start test thread1 fail:%d", ret);
            ret = HDF_FAILURE;
            break;
        }

        ret = OsalThreadStart(&thread2, &cfg2);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("start test thread2 fail:%d", ret);
            ret = HDF_FAILURE;
            break;
        }

        while (g_theard1Flag == false || g_theard2Flag == false) {
            HDF_LOGD("[%d]waitting testing timer thread finish...", time);
            OsalSleep(TIMER_TEST_WAIT_TIMEOUT);
            time++;
            if (time > TIMER_TEST_WAIT_TIMES) {
                break;
            }
        }
        ret = HDF_SUCCESS;
    } while (0);

    if (handle1 != NULL) {
        HwTimerClose(handle1);
        handle1 = NULL;
    }
    if (handle2 != NULL) {
        HwTimerClose(handle2);
        handle2 = NULL;
    }
    if (thread1.realThread != NULL) {
        (void)OsalThreadDestroy(&thread1);
    }
    if (thread2.realThread != NULL) {
        (void)OsalThreadDestroy(&thread2);
    }
    g_theard1Flag = false;
    g_theard2Flag = false;
    return ret;
}

int32_t TimerTestReliability(struct TimerTest *test)
{
    if (test == NULL || test->handle == NULL) {
        HDF_LOGE("%s: test null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HwTimerSet(test->handle, test->uSecond, NULL);
    HwTimerSetOnce(test->handle, test->uSecond, NULL);
    HwTimerStart(NULL);
    return HDF_SUCCESS;
}

static int32_t TimerTestGetConfig(struct TimerTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    if (config == NULL) {
        HDF_LOGE("%s: param null", __func__);
        return HDF_FAILURE;
    }

    service = HdfIoServiceBind("TIMER_TEST");
    if (service == NULL) {
        HDF_LOGE("%s: service TIMER_TEST bind fail", __func__);
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
        HdfSbufRecycle(reply);
        return ret;
    }

    if (!HdfSbufReadBuffer(reply, &buf, &len)) {
        HDF_LOGE("%s: read buf failed", __func__);
        HdfSbufRecycle(reply);
        HdfIoServiceRecycle(service);
        return HDF_ERR_IO;
    }

    if (len != sizeof(*config)) {
        HDF_LOGE("%s: config size:%zu, read size:%u", __func__, sizeof(*config), len);
        HdfSbufRecycle(reply);
        HdfIoServiceRecycle(service);
        return HDF_ERR_IO;
    }

    if (memcpy_s(config, sizeof(*config), buf, sizeof(*config)) != EOK) {
        HDF_LOGE("%s: memcpy buf failed", __func__);
        HdfSbufRecycle(reply);
        HdfIoServiceRecycle(service);
        return HDF_ERR_IO;
    }

    HdfSbufRecycle(reply);
    HdfIoServiceRecycle(service);
    return HDF_SUCCESS;
}

struct TimerTest *TimerTestGet(void)
{
    int32_t ret;
    static struct TimerTest tester;
    static bool hasInit = false;

    if (hasInit) {
        return &tester;
    }

    struct TimerTestConfig config;
    ret = TimerTestGetConfig(&config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed:%d", __func__, ret);
        return NULL;
    }

    tester.number = config.number;
    tester.uSecond = config.uSecond;
    tester.isPeriod = config.isPeriod;
    hasInit = true;
    return &tester;
}

static int32_t TimerIfPerformanceTest(struct TimerTest *test)
{
#ifdef __LITEOS__
    // liteos the accuracy of the obtained time is too large and inaccurate.
    if (test == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
#endif

    uint64_t startMs;
    uint64_t endMs;
    uint64_t useTime;    // ms
    uint32_t uSecond;
    bool isPeriod;

    startMs = OsalGetSysTimeMs();
    HwTimerGet(test->handle, &uSecond, &isPeriod);
    endMs = OsalGetSysTimeMs();

    useTime = endMs - startMs;
    HDF_LOGI("----->interface performance test:[start:%llu(ms) - end:%llu(ms) = %llu (ms)] < 1ms[%d]\r\n",
        startMs, endMs, useTime, useTime < 1 ? true : false);
    return HDF_SUCCESS;
}

static struct TimerTestFunc g_timerTestFunc[] = {
    {TIMER_TEST_SET, TimerSetTest},
    {TIMER_TEST_SETONCE, TimerSetOnceTest},
    {TIMER_TEST_GET, TimerGetTest},
    {TIMER_TEST_START, TimerStartTest},
    {TIMER_TEST_STOP, TimerStopTest},
    {TIMER_MULTI_THREAD_TEST, TimerTestMultiThread},
    {TIMER_RELIABILITY_TEST, TimerTestReliability},
    {TIMER_IF_PERFORMANCE_TEST, TimerIfPerformanceTest},
};

int32_t TimerTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;
    
    if (cmd > TIMER_TEST_MAX_CMD) {
        HDF_LOGE("%s: invalid cmd:%d", __func__, cmd);
        return HDF_ERR_NOT_SUPPORT;
    }

    struct TimerTest *test = TimerTestGet();
    if (test == NULL) {
        HDF_LOGE("%s: test null cmd %d", __func__, cmd);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cmd != TIMER_MULTI_THREAD_TEST) {
        test->handle = HwTimerOpen(test->number);
        if (test->handle == NULL) {
            HDF_LOGE("%s: timer test get handle fail", __func__);
            return HDF_FAILURE;
        }
    }

    for (i = 0; i < sizeof(g_timerTestFunc) / sizeof(g_timerTestFunc[0]); i++) {
        if (cmd == g_timerTestFunc[i].type && g_timerTestFunc[i].Func != NULL) {
            ret = g_timerTestFunc[i].Func(test);
            break;
        }
    }

    if (cmd != TIMER_MULTI_THREAD_TEST) {
        HwTimerClose(test->handle);
    }

    return ret;
}
