/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "securec.h"
#include "hdf_log.h"
#include "hdf_workqueue.h"
#include "osal_atomic.h"
#include "osal_file.h"
#include "osal_case_cmd_test.h"
#include "osal_time.h"

#define HDF_LOG_TAG osal_lite

#define MSG_SEND_PERIOD 5
#define MSG_NOSEND_CNT 1
#define MSG_NOSEND_CHECK_CNT 2
#define MSG_EVENT_AGAIN 3
#define MSG_EVENT_AGAIN_CHECK 4

#define TEST_ITEM_HAVE_EVENT 0
#define TEST_ITEM_NO_EVENT 1
#define TEST_ITEM_HAVE_EVENT_TIMEOUT 2

#define OSAL_TEST_PERIOD 20
static bool g_osalWorkStatus = true;
static bool g_osalDelayStatus = true;
static HdfWork g_work;
static HdfWorkQueue g_workQue;

static HdfWork g_delayWork;
static uint32_t g_workPara = 1000;
static int g_delayTime = 100;
#define DELAY_TIME_DEFAULT 100
#define DELAY_TIME_FOR_OUT 10000

static void TestWorkEntry(void *para)
{
    static int cnt = 0;
    if (para != NULL) {
        if (cnt % DELAY_TIME_DEFAULT == 0) {
            HDF_LOGE("%s %u", __func__, *(uint32_t *)para);
        }
        cnt++;
    }
    g_osalWorkStatus = false;
}

static void TestDelayWorkEntry(void *para)
{
    static int cnt = 0;

    if (para != NULL) {
        if (cnt % DELAY_TIME_DEFAULT == 0) {
            HDF_LOGE("%s %u", __func__, *(uint32_t *)para);
        }
        cnt++;
    }

    g_osalDelayStatus = false;
}

static void TestWorkInit(void)
{
    HdfWorkQueueInit(&g_workQue, "osal_workQueue");

    HdfWorkInit(&g_work, TestWorkEntry, (void *)&g_workPara);

    HdfDelayedWorkInit(&g_delayWork, TestDelayWorkEntry, (void *)&g_workPara);
}

void TestAddRoute(int cnt)
{
    if (cnt % MSG_SEND_PERIOD == 0) {
        g_osalWorkStatus = true;
        HdfAddWork(&g_workQue, &g_work);
        g_osalDelayStatus = true;
        g_delayTime = DELAY_TIME_DEFAULT;
        HdfAddDelayedWork(&g_workQue, &g_delayWork, g_delayTime);
    } else if (cnt % MSG_SEND_PERIOD == TEST_ITEM_NO_EVENT) {
        UT_TEST_CHECK_RET(g_osalWorkStatus, OSAL_WORK_RUN_CHECK);
        UT_TEST_CHECK_RET(g_osalDelayStatus, OSAL_DELAY_WORK_RUN_CHECK);
    } else if (cnt % MSG_SEND_PERIOD == MSG_NOSEND_CHECK_CNT) {
        g_delayTime = DELAY_TIME_FOR_OUT;
        g_osalDelayStatus = true;
        HdfAddDelayedWork(&g_workQue, &g_delayWork, g_delayTime);
        g_osalWorkStatus = true;
        HdfAddWork(&g_workQue, &g_work);
        HdfCancelWorkSync(&g_work);
    } else if (cnt % MSG_SEND_PERIOD == MSG_EVENT_AGAIN) {
        HdfCancelDelayedWorkSync(&g_delayWork);
    } else if (cnt % MSG_SEND_PERIOD == MSG_EVENT_AGAIN_CHECK) {
        UT_TEST_CHECK_RET(g_osalDelayStatus == false, OSAL_DELAY_WORK_RUN_CHECK);
    }
}

#define ATOMIC_INC_VALUE 2
static void OsalTestAtomic(void)
{
    OsalAtomic atomic;
    unsigned long testBits = 0;
    int32_t value;

    HDF_LOGE("%s test begin", __func__);

    OsalAtomicSet(&atomic, 1);
    value = OsalAtomicRead(&atomic);
    UT_TEST_CHECK_RET(value != 1, OSAL_ATOMIC_SET);
    UT_TEST_CHECK_RET(value != 1, OSAL_ATOMIC_READ);
    HDF_LOGE("%s %d %d", __func__, __LINE__, value);

    OsalAtomicInc(&atomic);
    value = OsalAtomicRead(&atomic);
    UT_TEST_CHECK_RET(value != ATOMIC_INC_VALUE, OSAL_ATOMIC_INC);
    UT_TEST_CHECK_RET(value != ATOMIC_INC_VALUE, OSAL_ATOMIC_READ);
    HDF_LOGE("%s %d %d", __func__, __LINE__, value);

    OsalAtomicDec(&atomic);
    value = OsalAtomicRead(&atomic);
    UT_TEST_CHECK_RET(value != 1, OSAL_ATOMIC_DEC);
    UT_TEST_CHECK_RET(value != 1, OSAL_ATOMIC_READ);

    HDF_LOGE("%s %d %d", __func__, __LINE__, value);
    value = OsalTestBit(1, &testBits);
    UT_TEST_CHECK_RET(value != 0, OSAL_TEST_BIT);
    HDF_LOGE("%s %d %d", __func__, __LINE__, value);

    value = OsalTestSetBit(1, &testBits);
    UT_TEST_CHECK_RET(value != 0, OSAL_TEST_SET_BIT);
    HDF_LOGE("%s %d %d", __func__, __LINE__, value);

    value = OsalTestBit(1, &testBits);
    UT_TEST_CHECK_RET(value != 1, OSAL_TEST_BIT);
    HDF_LOGE("%s %d %d", __func__, __LINE__, value);

    value = OsalTestClearBit(1, &testBits);
    UT_TEST_CHECK_RET(value != 1, OSAL_TEST_CLR_BIT);
    HDF_LOGE("%s %d %d", __func__, __LINE__, value);

    value = OsalTestBit(1, &testBits);
    UT_TEST_CHECK_RET(value != 0, OSAL_TEST_BIT);
    HDF_LOGE("%s %d %d", __func__, __LINE__, value);

    OsalClearBit(1, &testBits);
    value = OsalTestBit(1, &testBits);
    UT_TEST_CHECK_RET(value != 0, OSAL_CLR_BIT);

    HDF_LOGE("%s %d %d", __func__, __LINE__, value);
}

#define TEST_FILE_WRITE_CHAR 0xfe
#define TEST_FILE_OFFSET 10
#define TEST_FILE_LEN 72
static void OsalTestFile(int flag)
{
    OsalFile file;
    char buf[TEST_FILE_LEN];
    int32_t len;

    HDF_LOGE("%s test begin", __func__);
    if (flag != 0) {
        HDF_LOGE("%s file do not exist %d", __func__, flag);
        return;
    }
    (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
    UT_TEST_CHECK_RET(OsalFileOpen(&file, TEST_FILE_PATH_NAME, OSAL_O_RD_ONLY, OSAL_S_IREAD) != HDF_SUCCESS,
        OSAL_FILE_OPEN_RD);
    len = OsalFileRead(&file, buf, sizeof(buf));
    UT_TEST_CHECK_RET(len != TEST_FILE_LEN, OSAL_FILE_READ_RD);
    HDF_LOGE("%s %d %d", __func__, __LINE__, len);
    len = OsalFileLseek(&file, 0, 0);
    UT_TEST_CHECK_RET(len == -1, OSAL_FILE_SEEK_RD);
    (void)memset_s(buf, sizeof(buf), TEST_FILE_WRITE_CHAR, sizeof(buf));
    len = OsalFileWrite(&file, buf, sizeof(buf));
    UT_TEST_CHECK_RET(len != -1, OSAL_FILE_WRITE_RD);
    HDF_LOGE("%s %d %d", __func__, __LINE__, len);
    len = OsalFileLseek(&file, TEST_FILE_OFFSET, 0);
    UT_TEST_CHECK_RET(len != TEST_FILE_OFFSET, OSAL_FILE_SEEK_RD);
    len = OsalFileRead(&file, buf, sizeof(buf));
    UT_TEST_CHECK_RET(len != (TEST_FILE_LEN - TEST_FILE_OFFSET), OSAL_FILE_READ_RD);
    HDF_LOGE("%s %d %d", __func__, __LINE__, len);
    OsalFileClose(&file);
    len = OsalFileRead(&file, buf, sizeof(buf));
    UT_TEST_CHECK_RET(len >= 0, OSAL_FILE_READ_RD);
    HDF_LOGE("%s %d %d", __func__, __LINE__, len);

    (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
    UT_TEST_CHECK_RET(OsalFileOpen(&file, TEST_FILE_PATH_NAME, OSAL_O_RDWR, OSAL_S_IREAD) != HDF_SUCCESS,
        OSAL_FILE_OPEN_RDWR);
    len = OsalFileRead(&file, buf, sizeof(buf));
    UT_TEST_CHECK_RET(len != TEST_FILE_LEN, OSAL_FILE_READ_RDWR);
    HDF_LOGE("%s %d %d", __func__, __LINE__, len);
    len = OsalFileLseek(&file, 0, 0);
    UT_TEST_CHECK_RET(len == -1, OSAL_FILE_SEEK_RDWR);
    (void)memset_s(buf, sizeof(buf), TEST_FILE_WRITE_CHAR, sizeof(buf));
    len = OsalFileWrite(&file, buf, sizeof(buf));
    UT_TEST_CHECK_RET(len == -1, OSAL_FILE_WRITE_RDWR);
    HDF_LOGE("%s %d %d", __func__, __LINE__, len);
    len = OsalFileLseek(&file, TEST_FILE_OFFSET, 0);
    UT_TEST_CHECK_RET(len != TEST_FILE_OFFSET, OSAL_FILE_SEEK_RDWR);
    len = OsalFileRead(&file, buf, sizeof(buf));
    UT_TEST_CHECK_RET(len != (TEST_FILE_LEN - TEST_FILE_OFFSET), OSAL_FILE_READ_RDWR);
    HDF_LOGE("%s %d %d", __func__, __LINE__, len);
    OsalFileClose(&file);
}

void OsalTestWork(int flag)
{
    TestWorkInit();
    OsalTestAtomic();
    OsalTestFile(flag);
}

void OsalTestWorkEnd(void)
{
    HdfCancelWorkSync(&g_work);
    HdfWorkDestroy(&g_work);
    HdfCancelDelayedWorkSync(&g_delayWork);
    HdfDelayedWorkDestroy(&g_delayWork);
    HdfWorkQueueDestroy(&g_workQue);
}