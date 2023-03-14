/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_firmware.h"
#include "osal_irq.h"
#include "osal_list_test.h"
#include "osal_file_test.h"
#include "osal_case_cmd_test.h"
#include "osal_get_case_test.h"
#include "osal_test_case_def.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "osal_spinlock.h"
#ifndef __USER__
#include "osal_test_type.h"
#endif
#include "osal_thread.h"
#include "osal_time.h"
#include "osal_timer.h"
#include "securec.h"

#define IRQ_NUM_TEST 33
#define HDF_LOG_TAG osal_test

#define OSAL_TEST_TIME_DEFAULT 100

uint32_t g_osalTestCases[OSAL_TEST_CASE_CNT];

OsalTimespec g_hdfTestBegin;
OsalTimespec g_hdfTestEnd;
static int32_t g_waitMutexTime = 3100;
static int32_t g_threadTest1Flag = true;
OSAL_DECLARE_THREAD(thread1);
OSAL_DECLARE_THREAD(thread2);
OSAL_DECLARE_THREAD(thread);
struct OsalMutex g_mutexTest;
OSAL_DECLARE_SPINLOCK(g_spinTest);

#define HDF_THREAD_TEST_SLEEP_S 1
#define HDF_THREAD_TEST_SLEEP_US 600
#define HDF_THREAD_TEST_SLEEP_MS 300
#define HDF_THREAD_TEST_MUX_CNT 20
static int g_thread1RunFlag;
static int g_threadMuxLockFlag;
static int32_t g_test1Para = 120;
static int32_t g_test2Para = 123;
#define TIME_RANGE 200000
static bool OsalCheckTime(OsalTimespec *time, uint32_t ms)
{
    uint64_t t1 = time->sec * HDF_KILO_UNIT * HDF_KILO_UNIT + time->usec;
    uint64_t t2 = (uint64_t)ms * HDF_KILO_UNIT;
    uint64_t diff = (t1 < t2) ? (t2 - t1) : (t1 - t2);

    return diff < TIME_RANGE;
}

static int ThreadTest1(void *arg)
{
    static int cnt = 0;
    int ret;

    HDF_LOGI("[OSAL_UT_TEST]%s test thread para end", __func__);
    (void)arg;

    g_thread1RunFlag = true;

    while (g_threadTest1Flag) {
        OsalSleep(HDF_THREAD_TEST_SLEEP_S);
        HDF_LOGE("%s %d", __func__, cnt);
        cnt++;
        if (cnt > HDF_THREAD_TEST_MUX_CNT) {
            g_waitMutexTime = HDF_WAIT_FOREVER;
        }
        ret = OsalMutexTimedLock(&g_mutexTest, g_waitMutexTime);
        if (g_threadMuxLockFlag == true) {
            UT_TEST_CHECK_RET(ret == HDF_FAILURE, OSAL_MUTEX_LOCK_TIMEOUT);
        } else {
            UT_TEST_CHECK_RET(ret == HDF_FAILURE, OSAL_MUTEX_LOCK_TIMEOUT);
        }

        OsalMSleep(HDF_THREAD_TEST_SLEEP_MS);
        if (ret == HDF_SUCCESS) {
            ret = OsalMutexUnlock(&g_mutexTest);
            UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_UNLOCK);
        }
        OsalMSleep(HDF_THREAD_TEST_SLEEP_US);

        ret = OsalSpinLock(&g_spinTest);
        UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_LOCK);
        ret = OsalSpinUnlock(&g_spinTest);
        UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_UNLOCK);

        OsalMSleep(HDF_THREAD_TEST_SLEEP_MS);
        if (cnt % HDF_THREAD_TEST_MUX_CNT == 0) {
            HDF_LOGE("%s ", __func__);
        }
        if (g_testEndFlag) {
            break;
        }
    }
    HDF_LOGE("%s thread return\n", __func__);
    return 0;
}

static int g_thread2RunFlag;
static int32_t g_threadTest2Flag = true;
int ThreadTest2(void *arg)
{
    static int cnt = 0;
    OsalTimespec hdfTs1 = { 0, 0 };
    OsalTimespec hdfTs2 = { 0, 0 };
    OsalTimespec hdfTsDiff = { 0, 0 };
    int ret;
    int32_t para;

    HDF_LOGI("[OSAL_UT_TEST]%s test thread para end", __func__);
    if (arg != NULL) {
        para = *(int32_t *)arg;
        UT_TEST_CHECK_RET(para != g_test2Para, OSAL_THREAD_PARA_CHECK);
    } else {
        UT_TEST_CHECK_RET(true, OSAL_THREAD_PARA_CHECK);
    }

    g_thread2RunFlag = true;

    while (g_threadTest2Flag) {
        OsalSleep(HDF_THREAD_TEST_SLEEP_S);
        OsalGetTime(&hdfTs1);
        HDF_LOGE("%s %d", __func__, cnt);

        cnt++;
        ret = OsalMutexTimedLock(&g_mutexTest, g_waitMutexTime);
        if (g_threadMuxLockFlag == true) {
            UT_TEST_CHECK_RET(ret == HDF_FAILURE, OSAL_MUTEX_LOCK_TIMEOUT);
        } else {
            UT_TEST_CHECK_RET(ret == HDF_FAILURE, OSAL_MUTEX_LOCK_TIMEOUT);
        }

        OsalMSleep(HDF_THREAD_TEST_SLEEP_MS);
        if (ret == HDF_SUCCESS) {
            ret = OsalMutexUnlock(&g_mutexTest);
            UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_UNLOCK);
        }

        OsalMSleep(HDF_THREAD_TEST_SLEEP_US);

        ret = OsalSpinLock(&g_spinTest);
        UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_LOCK);
        ret = OsalSpinUnlock(&g_spinTest);
        UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_UNLOCK);

        OsalMSleep(HDF_THREAD_TEST_SLEEP_MS);
        OsalGetTime(&hdfTs2);
        OsalDiffTime(&hdfTs1, &hdfTs2, &hdfTsDiff);
        if (cnt % HDF_THREAD_TEST_MUX_CNT == 0) {
            HDF_LOGE("%s %us %uus", __func__,
                (uint32_t)hdfTsDiff.sec, (uint32_t)hdfTsDiff.usec);
        }
        if (g_testEndFlag) {
            break;
        }
    }
    HDF_LOGE("%s thread return\n", __func__);
    return 0;
}

#define HDF_DBG_CNT_CTRL 10
#ifndef __USER__
OSAL_DECLARE_TIMER(g_testTimerLoop1);
OSAL_DECLARE_TIMER(g_testTimerLoop2);
OSAL_DECLARE_TIMER(g_testTimerOnce);
#define HDF_TIMER1_PERIOD 630
#define HDF_TIMER2_PERIOD 1350
static int32_t g_timerPeriod1 = HDF_TIMER1_PERIOD;
static int32_t g_timerPeriod2 = HDF_TIMER2_PERIOD;
static int32_t g_timerPeriod3 = 10300;
static int32_t g_timerPeriod1Modify = 1250;
static int32_t g_timerPeriod2Modify = 750;
#define HDF_TIMER_TEST_MODIFY 8
#define HDF_TEST_TIMER_PARA 1
#define HDF_TEST_TIMER_MODIFY 2
#define HDF_TEST_TIMER_END 3

static int g_timerLoop1RunFlag;
static int g_timer1Cnt = 0;
static void TimerLoopTest1(uintptr_t arg)
{
    int32_t para;
    OsalTimespec hdfTs1 = { 0, 0 };
    static OsalTimespec hdfTs2 = { 0, 0 };
    OsalTimespec hdfTsDiff = { 0, 0 };
    static int index = HDF_TEST_TIMER_PARA;
    int32_t ret;

    if (g_timer1Cnt == 0) {
        OsalGetTime(&hdfTs2);
    }

    para = *(int32_t *)arg;
    if ((g_timer1Cnt >= 1) && (g_timer1Cnt != HDF_TIMER_TEST_MODIFY + 1)) {
        OsalGetTime(&hdfTs1);
        OsalDiffTime(&hdfTs2, &hdfTs1, &hdfTsDiff);
        HDF_LOGE("%s %d %d %d %d %d", __func__, g_timer1Cnt, para, (int32_t)hdfTsDiff.sec,
            (int32_t)hdfTsDiff.usec, g_timerPeriod1);
        UT_TEST_CHECK_RET(!OsalCheckTime(&hdfTsDiff, g_timerPeriod1), OSAL_TIMER_PERIOD_CHECK);
        UT_TEST_CHECK_RET(g_timerPeriod1 != para, OSAL_TIMER_PARA_CHECK);
        if (index == HDF_TEST_TIMER_PARA) {
            HDF_LOGE("[OSAL_UT_TEST]%s test timer para end", __func__);
            index = HDF_TEST_TIMER_END;
        }
        if (index == HDF_TEST_TIMER_MODIFY) {
            HDF_LOGE("[OSAL_UT_TEST]%s test timer modify function end", __func__);
            index = HDF_TEST_TIMER_END;
        }
    }

    if (g_timer1Cnt == HDF_TIMER_TEST_MODIFY) {
        g_timerPeriod1 = g_timerPeriod1Modify;
        ret = OsalTimerSetTimeout(&g_testTimerLoop1, g_timerPeriod1);
        UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIMER_MODIFY_CHECK);
        index = HDF_TEST_TIMER_MODIFY;
    }

    OsalGetTime(&hdfTs2);

    g_timer1Cnt++;
    g_timerLoop1RunFlag = true;
}

static int g_timerLoop2RunFlag;
static int g_timer2Cnt = 0;
static void TimerLoopTest2(uintptr_t arg)
{
    int32_t para;
    OsalTimespec hdfTs1 = { 0, 0 };
    static OsalTimespec hdfTs2 = { 0, 0 };
    OsalTimespec hdfTsDiff = { 0, 0 };
    static int index = HDF_TEST_TIMER_PARA;
    int32_t ret;

    if (g_timer2Cnt == 0) {
        OsalGetTime(&hdfTs2);
    }

    para = *(int32_t *)arg;
    if ((g_timer2Cnt >= 1) && (g_timer2Cnt != HDF_TIMER_TEST_MODIFY + 1)) {
        OsalGetTime(&hdfTs1);
        OsalDiffTime(&hdfTs2, &hdfTs1, &hdfTsDiff);
        HDF_LOGE("%s %d %d %d %d %d", __func__, g_timer2Cnt, para, (int32_t)hdfTsDiff.sec,
            (int32_t)hdfTsDiff.usec, g_timerPeriod2);

        UT_TEST_CHECK_RET(!OsalCheckTime(&hdfTsDiff, g_timerPeriod2), OSAL_TIMER_PERIOD_CHECK);
        UT_TEST_CHECK_RET(g_timerPeriod2 != para, OSAL_TIMER_PARA_CHECK);
        if (index == HDF_TEST_TIMER_PARA) {
            HDF_LOGE("[OSAL_UT_TEST]%s test timer para end", __func__);
            index = HDF_TEST_TIMER_END;
        }
        if (index == HDF_TEST_TIMER_MODIFY) {
            HDF_LOGE("[OSAL_UT_TEST]%s test timer modify function end", __func__);
            index = HDF_TEST_TIMER_END;
        }
    }

    if (g_timer2Cnt == HDF_TIMER_TEST_MODIFY) {
        g_timerPeriod2 = g_timerPeriod2Modify;
        HDF_LOGE("[OSAL_UT_TEST]%s modify timer", __func__);
        ret = OsalTimerSetTimeout(&g_testTimerLoop2, g_timerPeriod2);
        UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIMER_MODIFY_CHECK);
        index = HDF_TEST_TIMER_MODIFY;
    }
    OsalGetTime(&hdfTs2);
    g_timer2Cnt++;
    g_timerLoop2RunFlag = true;
}

static int g_timerOnceRunFlag;
static void TimerOnceTest(uintptr_t arg)
{
    int32_t para;
    para = *(int32_t *)arg;

    HDF_LOGE("%s %d", __func__, para);
    UT_TEST_CHECK_RET(para != g_timerPeriod3, OSAL_TIMER_PARA_CHECK);
    g_timerOnceRunFlag++;
}

static void OsaTimerTest(void)
{
    int32_t ret;

    HDF_LOGI("[OSAL_UT_TEST]%s start", __func__);
    g_timerPeriod1 = HDF_TIMER1_PERIOD;
    g_timerPeriod2 = HDF_TIMER2_PERIOD;

    ret = OsalTimerCreate(&g_testTimerLoop1, g_timerPeriod1, TimerLoopTest1, (uintptr_t)&g_timerPeriod1);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIMER_CREATE_LOOP);
    OsalTimerStartLoop(&g_testTimerLoop1);

    ret = OsalTimerCreate(&g_testTimerLoop2, g_timerPeriod2, TimerLoopTest2, (uintptr_t)&g_timerPeriod2);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIMER_CREATE_LOOP);
    OsalTimerStartLoop(&g_testTimerLoop2);

    ret = OsalTimerCreate(&g_testTimerOnce, g_timerPeriod3, TimerOnceTest, (uintptr_t)&g_timerPeriod3);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIMER_CREATE_ONCE);
    OsalTimerStartOnce(&g_testTimerOnce);

    HDF_LOGI("[OSAL_UT_TEST]%s end", __func__);
}

#define HDF_ONCE_TIMER_DEL_TIME 10
static void OsaTimerTestStop(void)
{
    int32_t ret;

    HDF_LOGI("[OSAL_UT_TEST]%s start", __func__);
    ret = OsalTimerDelete(&g_testTimerLoop2);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIMER_STOP_CHECK);
    g_timerLoop2RunFlag = false;

    ret = OsalTimerDelete(&g_testTimerOnce);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIMER_STOP_CHECK);

    ret = OsalTimerCreate(&g_testTimerOnce, g_timerPeriod3, TimerOnceTest, (uintptr_t)&g_timerPeriod3);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIMER_CREATE_ONCE);

    OsalTimerStartOnce(&g_testTimerOnce);
    HDF_LOGI("[OSAL_UT_TEST]%s OsalTimerStartOnce", __func__);
    OsalMSleep(HDF_ONCE_TIMER_DEL_TIME);
    ret = OsalTimerDelete(&g_testTimerOnce);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIMER_STOP_CHECK);

    HDF_LOGI("[OSAL_UT_TEST]%s end", __func__);
}

#define HDF_FILE_SEEK_TEST 8
static void OsaFWTest(int flag)
{
    struct OsalFirmware fw;
    struct OsalFwBlock block;
    int32_t ret;
    char *name = NULL;
    char *fwBuf = OsalFWTestBuff();

    HDF_LOGE("[OSAL_UT_TEST]%s start", __func__);
    if (flag != 0) {
        HDF_LOGE("%s file do not exist %d", __func__, flag);
        return;
    }
#if defined(__LITEOS__)
    name = TEST_FW_PATH_NAME;
#else
    name = TEST_FW_NAME;
#endif
    (void)memset_s(&fw, sizeof(fw), 0, sizeof(fw));
    ret = OsalRequestFirmware(&fw, name, NULL);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_FW_REQUEST);
    HDF_LOGE("%s %u", name, fw.fwSize);

    (void)memset_s(&block, sizeof(block), 0, sizeof(block));
    ret = OsalReadFirmware(&fw, &block);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_FW_READ);
    HDF_LOGE("%s %d %d", name, block.dataSize, block.curOffset);
    UT_TEST_CHECK_RET(memcmp(block.data, fwBuf, block.dataSize) != 0, OSAL_FW_DATA_CHECK);
#if defined(__LITEOS__)
    ret = OsalSeekFirmware(&fw, HDF_FILE_SEEK_TEST);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_FW_SEEK);
    HDF_LOGE("%s %u %d", name, block.dataSize, block.curOffset);
    ret = OsalReadFirmware(&fw, &block);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_FW_READ);
    HDF_LOGE("%s %u %d", name, block.dataSize, block.curOffset);
    fwBuf += HDF_FILE_SEEK_TEST;
    UT_TEST_CHECK_RET(memcmp(block.data, fwBuf, block.dataSize) != 0, OSAL_FW_DATA_CHECK);
#endif
    ret = OsalReleaseFirmware(&fw);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_FW_RELEASE);
    ret = OsalReadFirmware(&fw, &block);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_FW_READ_AFTER_RELEASE);
    HDF_LOGE("[OSAL_UT_TEST]%s end", __func__);
}

#define THREAD_TEST_TIMER_RUN 20
#define THREAD_TEST_TIMER_STOP 25

static void OsaCheckRun(int cnt)
{
    OsalTimespec diffTime = { 0, 0 };

    if (cnt == THREAD_TEST_TIMER_RUN) {
        UT_TEST_CHECK_RET(g_timerOnceRunFlag == 0, OSAL_TIMER_RUN_CHECK);
        UT_TEST_CHECK_RET(g_timerLoop2RunFlag == false, OSAL_TIMER_RUN_CHECK);
        UT_TEST_CHECK_RET(g_timerLoop1RunFlag == false, OSAL_TIMER_RUN_CHECK);
        HDF_LOGI("[OSAL_UT_TEST]%s timer run end", __func__);
    }

    if (cnt == THREAD_TEST_TIMER_STOP) {
        UT_TEST_CHECK_RET(g_timerOnceRunFlag != 1, OSAL_TIMER_STOP_CHECK);
        UT_TEST_CHECK_RET(g_timerLoop2RunFlag != false, OSAL_TIMER_STOP_CHECK);
        HDF_LOGI("[OSAL_UT_TEST]%s timer stop end", __func__);
    }
    if (cnt == THREAD_TEST_TIMER_STOP) {
        OsalGetTime(&g_hdfTestEnd);
        OsalDiffTime(&g_hdfTestBegin, &g_hdfTestEnd, &diffTime);
        HDF_LOGI("[OSAL_UT_TEST]%s **** All case test end, use %ds****", __func__, (uint32_t)diffTime.sec);
        HDF_LOGI("[OSAL_UT_TEST]%s ***************************", __func__);
    }
}
static uint8_t g_irqData = 100;
uint32_t IRQHandle(uint32_t irqId, void *data)
{
    uint8_t *p = data;
    static OsalTimespec hdfTs = { 0, 0 };
    OsalTimespec hdfTs2 = { 0, 0 };
    OsalTimespec hdfTsDiff = { 0, 0 };
    if (p == NULL) {
        HDF_LOGE("%s nul ptr", __func__);
        return 1;
    }

    OsalGetTime(&hdfTs2);
    OsalDiffTime(&hdfTs, &hdfTs2, &hdfTsDiff);

    HDF_LOGE("%s IRQ handle, irqId:%u, data:%u, %ds %dus", __func__, irqId, *p,
        (int32_t)(hdfTsDiff.sec), (int32_t)(hdfTsDiff.usec));
    OsalGetTime(&hdfTs);

    return 0;
}

#define IRQ_TEST_CNT 10
void OsaIrqTest(void)
{
    int i = 0;
    int32_t ret;
    int32_t retFlag = HDF_FAILURE;
#if defined(__LITEOS__)
    retFlag = HDF_SUCCESS;
#endif

    HDF_LOGI("[OSAL_UT_TEST]%s start", __func__);
#if defined(__LITEOS__)
    ret = OsalUnregisterIrq(IRQ_NUM_TEST, &g_irqData);
    UT_TEST_CHECK_RET(ret == retFlag, OSAL_IRQ_UNREG_ABNORMAL);
#endif
    ret = OsalRegisterIrq(IRQ_NUM_TEST, 0, IRQHandle, "test", &g_irqData);
    UT_TEST_CHECK_RET(ret != retFlag, OSAL_IRQ_REG_NORMAL);
    ret = OsalDisableIrq(IRQ_NUM_TEST);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_IRQ_REG_NORMAL);
    ret = OsalEnableIrq(IRQ_NUM_TEST);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_IRQ_REG_NORMAL);
#if defined(__LITEOS__)
    ret = OsalUnregisterIrq(IRQ_NUM_TEST, &g_irqData);
    UT_TEST_CHECK_RET(ret != retFlag, OSAL_IRQ_UNREG_NORMAL);
#endif

    ret = OsalDisableIrq(IRQ_NUM_TEST);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_IRQ_REG_NORMAL);

    ret = OsalEnableIrq(IRQ_NUM_TEST);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_IRQ_REG_NORMAL);

#if defined(__LITEOS__)
    ret = OsalUnregisterIrq(IRQ_NUM_TEST, &g_irqData);
    UT_TEST_CHECK_RET(ret == retFlag, OSAL_IRQ_UNREG_ABNORMAL);
#endif

    for (; i < IRQ_TEST_CNT; i++) {
        ret = OsalRegisterIrq(IRQ_NUM_TEST, 0, IRQHandle, "test", &g_irqData);
        UT_TEST_CHECK_RET(ret != retFlag, OSAL_IRQ_REG_NORMAL_STRESS);
        ret = OsalDisableIrq(IRQ_NUM_TEST);
        UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_IRQ_REG_NORMAL_STRESS);
        ret = OsalEnableIrq(IRQ_NUM_TEST);
        UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_IRQ_REG_NORMAL_STRESS);
#if defined(__LITEOS__)
        ret = OsalUnregisterIrq(IRQ_NUM_TEST, &g_irqData);
        UT_TEST_CHECK_RET(ret != retFlag, OSAL_IRQ_UNREG_NORMAL_STRESS);
#endif
    }

    HDF_LOGI("[OSAL_UT_TEST]%s end", __func__);
}

#define IRQ_TEST_REG 1
#define IRQ_TEST_ENABLE 10
#define IRQ_TEST_DISABLE 20
#define IRQ_TEST_UNREG 30
#define THREAD_TEST_STOP_TIMER 25
#define THREAD_TEST_STOP_TIMER_CHECK 45
#endif

#define THREAD_TEST_DBG_CTRL 200
#define THREAD_TEST_MUX_BEGIN 3
#define THREAD_TEST_MUX_END 5
#define THREAD_TEST_SLEEP_MS 1
static int g_thread3RunFlag;
static int32_t g_threadTestFlag = true;
static int ThreadTest(void *arg)
{
    static int cnt = 0;
    HDF_STATUS ret;
    HDF_LOGI("in threadTest %s %d", __func__, __LINE__);

    g_thread3RunFlag = true;
    (void)arg;

    while (g_threadTestFlag) {
        OsalSleep(THREAD_TEST_SLEEP_MS);

        if (cnt < THREAD_TEST_DBG_CTRL && cnt % HDF_DBG_CNT_CTRL == 0) {
            HDF_LOGI("in threadTest %d", cnt);
        }
        cnt++;

        if (cnt == THREAD_TEST_MUX_BEGIN) {
            HDF_LOGE("%s mutex Lock", __func__);
            g_threadMuxLockFlag = true;
            ret = OsalMutexTimedLock(&g_mutexTest, g_waitMutexTime);
            UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_STRESS_TEST);
        }
        if (cnt == THREAD_TEST_MUX_END) {
            ret = OsalMutexUnlock(&g_mutexTest);
            UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_STRESS_TEST);
            HDF_LOGE("%s mutex unLock", __func__);
            g_threadMuxLockFlag = false;
        }
#ifndef __USER__
        if (cnt == THREAD_TEST_STOP_TIMER) {
            OsaTimerTestStop();
        }
        OsaCheckRun(cnt);
#endif
        if (g_testEndFlag) {
            break;
        }
    }
    HDF_LOGE("%s thread return\n", __func__);
    return 0;
}

#define HDF_TEST_STACK_SIZE 10000
void OsaThreadTest1(void)
{
    struct OsalThreadParam threadCfg;
    static int para = 120;
    int32_t ret;

    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name = "hdf_test0";
    threadCfg.priority = OSAL_THREAD_PRI_HIGHEST;
    threadCfg.stackSize = HDF_TEST_STACK_SIZE;
    ret = OsalThreadCreate(&thread, (OsalThreadEntry)ThreadTest, (void *)&para);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);
    ret = OsalThreadStart(&thread, &threadCfg);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);
}

void OsaThreadTest(void)
{
    struct OsalThreadParam threadCfg;
    int ret;

    HDF_LOGI("[OSAL_UT_TEST]%s start", __func__);
    ret = OsalMutexInit(&g_mutexTest);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_CREATE);
    ret = OsalSpinInit(&g_spinTest);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_CREATE);

    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name = "hdf_test1";
    threadCfg.priority = OSAL_THREAD_PRI_HIGH;
    threadCfg.stackSize = HDF_TEST_STACK_SIZE;
    ret = OsalThreadCreate(&thread1, (OsalThreadEntry)ThreadTest1, (void *)&g_test1Para);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);
    ret = OsalThreadStart(&thread1, &threadCfg);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);

    OsalMSleep(HDF_THREAD_TEST_SLEEP_S);

    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name = "hdf_test2";
    threadCfg.priority = OSAL_THREAD_PRI_DEFAULT;
    threadCfg.stackSize = HDF_TEST_STACK_SIZE;
    ret = OsalThreadCreate(&thread2, (OsalThreadEntry)ThreadTest2, (void *)&g_test2Para);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);
    ret = OsalThreadStart(&thread2, &threadCfg);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);

    OsaThreadTest1();
    HDF_LOGI("[OSAL_UT_TEST]%s end", __func__);
}

#define TIME_TEST_SLEEP_S 2
#define TIME_TEST_SLEEP_MS 200
#define TIME_TEST_SLEEP_US 200
#define TIME_TEST_MS_RANGE_H 210
#define TIME_TEST_MS_RANGE_L 190
#define TIME_TEST_US_RANGE_H 1000
#define TIME_TEST_US_RANGE_L 200
static void OsaTimeTest(void)
{
    int32_t ret;
    OsalTimespec hdfTs = { 0, 0 };
    OsalTimespec hdfTs2 = { 0, 0 };
    OsalTimespec hdfTsDiff = { 0, 0 };

    OsalGetTime(&hdfTs);
    OsalSleep(TIME_TEST_SLEEP_S);
    ret = OsalGetTime(&hdfTs2);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIME_GETTIME);
    ret = OsalDiffTime(&hdfTs, &hdfTs2, &hdfTsDiff);
    HDF_LOGE("%s %us %uus", __func__, (uint32_t)hdfTsDiff.sec, (uint32_t)hdfTsDiff.usec);
    UT_TEST_CHECK_RET(!OsalCheckTime(&hdfTsDiff, TIME_TEST_SLEEP_S * HDF_KILO_UNIT), OSAL_TIME_DIFFTIME);

    OsalGetTime(&hdfTs);
    OsalUDelay(TIME_TEST_SLEEP_MS);
    (void)OsalGetTime(&hdfTs2);
    (void)OsalDiffTime(&hdfTs, &hdfTs2, &hdfTsDiff);
    HDF_LOGE("%s %us %uus", __func__, (uint32_t)hdfTsDiff.sec, (uint32_t)hdfTsDiff.usec);

    OsalGetTime(&hdfTs);
    OsalMDelay(TIME_TEST_SLEEP_MS);
    (void)OsalGetTime(&hdfTs2);
    (void)OsalDiffTime(&hdfTs, &hdfTs2, &hdfTsDiff);
    HDF_LOGE("%s %us %uus", __func__, (uint32_t)hdfTsDiff.sec, (uint32_t)hdfTsDiff.usec);

    OsalGetTime(&hdfTs);
    OsalMSleep(TIME_TEST_SLEEP_MS);
    ret = OsalGetTime(&hdfTs2);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIME_GETTIME);
    ret = OsalDiffTime(&hdfTs, &hdfTs2, &hdfTsDiff);
    HDF_LOGE("%s %us %uus", __func__, (uint32_t)hdfTsDiff.sec, (uint32_t)hdfTsDiff.usec);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_TIME_DIFFTIME);
    UT_TEST_CHECK_RET(!OsalCheckTime(&hdfTsDiff, TIME_TEST_SLEEP_MS), OSAL_TIME_DIFFTIME);
}
enum {
    MEM_ALIGN_TEST_0 = 1,
    MEM_ALIGN_TEST_1 = 2,
    MEM_ALIGN_TEST_2 = 3,
    MEM_ALIGN_TEST_3 = 4,
    MEM_ALIGN_TEST_4 = 7,
    MEM_ALIGN_TEST_5 = 8,
    MEM_ALIGN_TEST_6 = 16,
};
#define MALLOC_TEST_CASE1_SIZE 0x4000
#define MALLOC_TEST_CASE2_SIZE 0x1FF
#define MALLOC_TEST_CNT 1000
static void OsaMemoryTest(void)
{
    void *buf = NULL;
    int i = 0;

    HDF_LOGI("[OSAL_UT_TEST]%s start", __func__);
    buf = OsalMemCalloc(MALLOC_TEST_CASE1_SIZE);
    UT_TEST_CHECK_RET(buf == NULL, OSAL_MALLOC_BIG);
    OsalMemFree(buf);

    buf = OsalMemCalloc(MALLOC_TEST_CASE2_SIZE);
    UT_TEST_CHECK_RET(buf == NULL, OSAL_MALLOC_SMALL);
    OsalMemFree(buf);

    buf = OsalMemAllocAlign(MEM_ALIGN_TEST_0, MALLOC_TEST_CASE1_SIZE);
    UT_TEST_CHECK_RET(buf != NULL, OSAL_MALLOC_BIG);
    OsalMemFree(buf);
    buf = OsalMemAllocAlign(MEM_ALIGN_TEST_1, MALLOC_TEST_CASE1_SIZE);
    UT_TEST_CHECK_RET(buf != NULL, OSAL_MALLOC_BIG);
    OsalMemFree(buf);
    buf = OsalMemAllocAlign(MEM_ALIGN_TEST_2, MALLOC_TEST_CASE1_SIZE);
    UT_TEST_CHECK_RET(buf != NULL, OSAL_MALLOC_BIG);
    OsalMemFree(buf);
    buf = OsalMemAllocAlign(MEM_ALIGN_TEST_3, MALLOC_TEST_CASE1_SIZE);
    if (sizeof(void *) == MEM_ALIGN_TEST_3) {
        UT_TEST_CHECK_RET(buf == NULL, OSAL_MALLOC_BIG);
    } else {
        UT_TEST_CHECK_RET(buf != NULL, OSAL_MALLOC_BIG);
    }
    OsalMemFree(buf);
    buf = OsalMemAllocAlign(MEM_ALIGN_TEST_4, MALLOC_TEST_CASE1_SIZE);
    UT_TEST_CHECK_RET(buf != NULL, OSAL_MALLOC_BIG);
    OsalMemFree(buf);
    buf = OsalMemAllocAlign(MEM_ALIGN_TEST_5, MALLOC_TEST_CASE1_SIZE);
    UT_TEST_CHECK_RET(buf == NULL, OSAL_MALLOC_BIG);
    OsalMemFree(buf);
    buf = OsalMemAllocAlign(MEM_ALIGN_TEST_6, MALLOC_TEST_CASE1_SIZE);
    UT_TEST_CHECK_RET(buf == NULL, OSAL_MALLOC_BIG);
    OsalMemFree(buf);

    for (; i < MALLOC_TEST_CNT; i++) {
        buf = OsalMemCalloc(MALLOC_TEST_CASE1_SIZE);
        UT_TEST_CHECK_RET(buf == NULL, OSAL_MALLOC_BIG_STRESS);
        OsalMemFree(buf);

        buf = OsalMemCalloc(MALLOC_TEST_CASE2_SIZE);
        UT_TEST_CHECK_RET(buf == NULL, OSAL_MALLOC_SMALL_STRESS);
        OsalMemFree(buf);
    }
    HDF_LOGI("[OSAL_UT_TEST]%s end", __func__);
}

static void OsaLogTest(void)
{
    HDF_LOGI("[OSAL_UT_TEST]%s start", __func__);
    HDF_LOGE("{private}%{private}s %{private}d{private}", __func__, __LINE__);
    HDF_LOGW("%{public}s %{public}d", __func__, __LINE__);
    HDF_LOGI("%{public}s %{private}d", __func__, __LINE__);
    HDF_LOGI("%{private}s %{public}d", __func__, __LINE__);
    HDF_LOGD("%s", __func__);
    HDF_LOGV("%s", __func__);
    HDF_LOGI("[OSAL_UT_TEST]%{private}s end", __func__);
    HDF_LOGI("[OSAL_UT_TEST]%s end", __func__);
}

static void OsaMutexTest(void)
{
    HDF_STATUS ret;
    struct OsalMutex mutex;

    HDF_LOGE("[OSAL_UT_TEST]%s start", __func__);

    (void)memset_s(&mutex, sizeof(mutex), 0, sizeof(mutex));
    ret = OsalMutexInit(&mutex);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_CREATE);
    ret = OsalMutexTimedLock(&mutex, OSAL_TEST_TIME_DEFAULT);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_LOCK_TIMEOUT);
    ret = OsalMutexUnlock(&mutex);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_UNLOCK);
    ret = OsalMutexLock(&mutex);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_LOCK_FOREVER);
    ret = OsalMutexUnlock(&mutex);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_UNLOCK);
    ret = OsalMutexDestroy(&mutex);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_MUTEX_DESTROY);

    HDF_LOGE("[OSAL_UT_TEST]%s test no init", __func__);
    ret = OsalMutexTimedLock(&mutex, HDF_WAIT_FOREVER);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_MUTEX_VISIT_DESTROY);
    ret = OsalMutexUnlock(&mutex);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_MUTEX_VISIT_DESTROY);
    ret = OsalMutexDestroy(&mutex);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_MUTEX_VISIT_DESTROY);

    HDF_LOGI("[OSAL_UT_TEST]%s end", __func__);
}

static void OsaSpinTest(void)
{
    int32_t ret;
#ifndef __USER__
    uint32_t flag = 0;
#endif
    OSAL_DECLARE_SPINLOCK(spin);

    HDF_LOGE("[OSAL_UT_TEST]%s start", __func__);
    (void)memset_s(&spin, sizeof(spin), 0, sizeof(spin));
    ret = OsalSpinInit(&spin);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_CREATE);

    ret = OsalSpinLock(&spin);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_LOCK);
    ret = OsalSpinUnlock(&spin);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_UNLOCK);
#ifndef __USER__
    ret = OsalSpinLockIrq(&spin);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_LOCK_IRQ);
    ret = OsalSpinUnlockIrq(&spin);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_UNLOCK_IRQ);

    ret = OsalSpinLockIrqSave(&spin, &flag);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_LOCK_IRQ_SAVE);
    ret = OsalSpinUnlockIrqRestore(&spin, &flag);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_UNLOCK_IRQ_RESTORE);
#endif
    ret = OsalSpinDestroy(&spin);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SPIN_DESTROY);

    HDF_LOGE("[OSAL_UT_TEST]%s test no init", __func__);
    ret = OsalSpinLock(&spin);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_SPIN_LOCK_AFTER_DESTROY);
    ret = OsalSpinUnlock(&spin);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_SPIN_LOCK_AFTER_DESTROY);
    ret = OsalSpinDestroy(&spin);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_SPIN_LOCK_AFTER_DESTROY);

    HDF_LOGI("[OSAL_UT_TEST]%s end", __func__);
}

static void OsaCheckThreadRun(void)
{
    UT_TEST_CHECK_RET(g_thread1RunFlag == 0, OSAL_THREAD_RUN_CHECK);
    UT_TEST_CHECK_RET(g_thread1RunFlag == 0, OSAL_THREAD_RUN_CHECK);
    UT_TEST_CHECK_RET(g_thread1RunFlag == 0, OSAL_THREAD_RUN_CHECK);
    HDF_LOGI("[OSAL_UT_TEST]%s end", __func__);
}

#define HDF_MAIN_SLEEP_S 20
int OsaTestBegin(void)
{
    int ret;
    (void)memset_s(g_osalTestCases, sizeof(g_osalTestCases), 0, sizeof(g_osalTestCases));
    g_testEndFlag = false;
#ifndef __USER__
    g_timer1Cnt = 0;
    g_timer2Cnt = 0;
#endif
    OsalGetTime(&g_hdfTestBegin);
    OsaLogTest();
#ifndef __USER__
    ret = OsalTestFileInit();
#else
    ret = 0;
#endif
    OsalTestOther(ret);
    OsaTimeTest();
    OsaMutexTest();
    OsaSpinTest();
#ifndef __USER__
    OsaIrqTest();
    OsaTimerTest();
#endif
    OsaThreadTest();
    OsaMemoryTest();
    HDF_LOGD("%s ", __func__);
#ifndef __USER__
    OsaFWTest(ret);
    OsalTestFileDeInit();
#endif
    OsalSleep(HDF_MAIN_SLEEP_S);
    HDF_LOGI("%s", __func__);
    OsaCheckThreadRun();

    return 0;
}

int OsaTestEnd(void)
{
#ifndef __USER__
    OsalTimerDelete(&g_testTimerLoop1);
    OsalTimerDelete(&g_testTimerLoop2);
    OsalTimerDelete(&g_testTimerOnce);
    OsalStopThread();
#endif
    g_testEndFlag = true;
    OsalThreadDestroy(&thread1);
    OsalThreadDestroy(&thread);
    OsalThreadDestroy(&thread2);
    HDF_LOGE("%s", __func__);

    return 0;
}

int OsaTestALLResult(void)
{
    int index;
    for (index = 0; index < OSAL_TEST_CASE_CNT; index++) {
        if (g_osalTestCases[index] != 0) {
            HDF_LOGE("%s %d %d", __func__, g_osalTestCases[index], index);
            return 1;
        }
    }
    return 0;
}

