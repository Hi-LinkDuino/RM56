/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "securec.h"
#include "hdf_dlist.h"
#include "hdf_log.h"
#include "osal_case_cmd_test.h"
#include "osal_work_test.h"
#include "osal_sem.h"
#include "osal_thread.h"
#include "osal_time.h"

#define LIST_LEN_TAIL 10
#define LIST_LEN_HEAD 10

#define HDF_LOG_TAG osal_test

#define HDF_THREAD_TEST_SLEEP_MS 200
#define HDF_TEST_STACK_SIZE 10000

typedef void(*EventHandle)(int para);

typedef struct {
    EventHandle handle;
    struct DListHead list;
} OsalTestEventHandle;
bool g_testEndFlag = false;
static OsalTestEventHandle g_handleMng;
static OsalTestEventHandle g_handleArr[LIST_LEN_TAIL + LIST_LEN_HEAD];

static struct OsalSem g_hdfSem;
static struct OsalThread g_threadPostSem;
static struct OsalThread g_threadWaitSem;
static bool g_threadTestFlag = true;
static int32_t g_threadPara = 120;
static bool g_hdfListStatus = true;
static bool g_hdfSemStatus = true;

#define MSG_SEND_PERIOD 5
#define MSG_TEST_ITME_PERIOD 10

#define MSG_NOSEND_CNT 1
#define MSG_NOSEND_CHECK_CNT 2
#define MSG_EVENT_AGAIN 3

static int g_listCnt = 0;
static int g_listLen = LIST_LEN_TAIL + LIST_LEN_HEAD;
static void OsalEventHandlerTail(int para)
{
    static int cnt = 0;

    cnt += para;
    if ((g_listLen != 0) && (cnt % g_listLen) == 0) {
        HDF_LOGE("in %s msg:%d", __func__, cnt);
    }
    g_hdfListStatus = false;
    g_listCnt++;
}

static void OsalEventHandlerHead(int para)
{
    static int cnt = 0;

    cnt += para;
    if ((g_listLen != 0) && (cnt % g_listLen) == 0) {
        HDF_LOGE("in %s msg:%d", __func__, cnt);
    }
    g_hdfListStatus = false;
    g_listCnt++;
}

static void OsalTestList(const struct DListHead *head)
{
    OsalTestEventHandle *pos = NULL;
    g_listCnt = 0;
    DLIST_FOR_EACH_ENTRY(pos, head, OsalTestEventHandle, list) {
        if (pos->handle != NULL) {
            pos->handle(1);
        } else {
            HDF_LOGE("%s no handle\n", __func__);
            UT_TEST_CHECK_RET(true, OSAL_LIST_TRAVERSAL);
        }
    }
}

static void TestListAddTail(void)
{
    int i = 0;
    for (; i < LIST_LEN_TAIL; i++) {
        g_handleArr[i].handle = OsalEventHandlerTail;
        DListHeadInit(&g_handleArr[i].list);
        DListInsertTail(&g_handleArr[i].list, &g_handleMng.list);
    }
}

static void TestListMerge(void)
{
    OsalTestEventHandle list1[LIST_LEN_TAIL];
    OsalTestEventHandle list2[LIST_LEN_TAIL];
    OsalTestEventHandle mng1;
    OsalTestEventHandle mng2;
    int i = 0;

    HDF_LOGE("[OSAL_UT_TEST]%s start", __func__);

    DListHeadInit(&mng1.list);
    mng1.handle = NULL;
    for (; i < LIST_LEN_TAIL; i++) {
        list1[i].handle = OsalEventHandlerTail;
        DListHeadInit(&list1[i].list);
        DListInsertTail(&list1[i].list, &mng1.list);
    }
    OsalTestList(&mng1.list);
    UT_TEST_CHECK_RET(g_listCnt != LIST_LEN_TAIL, OSAL_LIST_TAIL);

    DListHeadInit(&mng2.list);
    mng2.handle = NULL;
    for (i = 0; i < LIST_LEN_TAIL; i++) {
        list2[i].handle = OsalEventHandlerTail;
        DListHeadInit(&list2[i].list);
        DListInsertTail(&list2[i].list, &mng2.list);
    }
    OsalTestList(&mng2.list);
    UT_TEST_CHECK_RET(g_listCnt != LIST_LEN_TAIL, OSAL_LIST_TAIL);

    DListMerge(&mng1.list, &mng2.list);
    OsalTestList(&mng2.list);
    UT_TEST_CHECK_RET(g_listCnt != g_listLen, OSAL_LIST_TAIL);

    OsalTestList(&mng1.list);
    UT_TEST_CHECK_RET(g_listCnt != 0, OSAL_LIST_TAIL);
    HDF_LOGE("[OSAL_UT_TEST]%s end", __func__);
}

static void TestListAddHead(void)
{
    int i = 0;
    for (; i < LIST_LEN_HEAD; i++) {
        g_handleArr[LIST_LEN_TAIL + i].handle = OsalEventHandlerHead;
        DListHeadInit(&g_handleArr[LIST_LEN_TAIL + i].list);
        DListInsertHead(&g_handleArr[LIST_LEN_TAIL + i].list, &g_handleMng.list);
    }
}

static void TestListRemoveInlist(void)
{
    OsalTestEventHandle *pos = NULL;
    OsalTestEventHandle *q = NULL;

    g_listCnt = 0;
    DLIST_FOR_EACH_ENTRY_SAFE(pos, q, &g_handleMng.list, OsalTestEventHandle, list) {
        if (&(pos->list) == &g_handleArr[0].list) {
            DListRemove(&(pos->list));
            g_listLen--;
            HDF_LOGE("%s find\n", __func__);
            continue;
        }
        if (pos->handle != NULL) {
            pos->handle(1);
        } else {
            HDF_LOGE("%s no handle\n", __func__);
            UT_TEST_CHECK_RET(true, OSAL_LIST_TRAVERSAL);
        }
    }
}

static void OsalTestListInit(void)
{
    int i = 0;
    HDF_LOGE("[OSAL_UT_TEST]%s start", __func__);

    g_handleMng.handle = NULL;
    DListHeadInit(&g_handleMng.list);
    UT_TEST_CHECK_RET(DListIsEmpty(&g_handleMng.list) != true, OSAL_LIST_INIT);
    OsalTestList(&g_handleMng.list);
    UT_TEST_CHECK_RET(g_listCnt != 0, OSAL_LIST_COUNT_CHECK);
    HDF_LOGE("g_listCnt %d %d", g_listCnt, g_listLen);

    TestListAddTail();
    UT_TEST_CHECK_RET(DListIsEmpty(&g_handleMng.list) == true, OSAL_LIST_TAIL);
    TestListAddHead();

    OsalTestList(&g_handleMng.list);
    UT_TEST_CHECK_RET(g_listCnt != g_listLen, OSAL_LIST_HEAD);
    HDF_LOGE("g_listCnt %d %d", g_listCnt, g_listLen);

    for (; i < g_listLen; i++) {
        DListRemove(&g_handleArr[i].list);
    }
    UT_TEST_CHECK_RET(DListIsEmpty(&g_handleMng.list) != true, OSAL_LIST_REMOVE);

    OsalTestList(&g_handleMng.list);
    UT_TEST_CHECK_RET(g_listCnt != 0, OSAL_LIST_TRAVERSAL);
    HDF_LOGE("g_listCnt %d %d", g_listCnt, g_listLen);

    TestListAddHead();
    TestListAddTail();

    OsalTestList(&g_handleMng.list);
    UT_TEST_CHECK_RET(g_listCnt != g_listLen, OSAL_LIST_TRAVERSAL);
    HDF_LOGE("g_listCnt %d %d", g_listCnt, g_listLen);

    TestListRemoveInlist();
    UT_TEST_CHECK_RET(g_listCnt != g_listLen, OSAL_LIST_TRAVERSAL_REMOVE);
    HDF_LOGE("g_listCnt %d %d", g_listCnt, g_listLen);

    DListInsertTail(&g_handleArr[0].list, &g_handleMng.list);
    g_listLen++;

    UT_TEST_CHECK_RET(DListIsEmpty(&g_handleMng.list) == true, OSAL_LIST_EMPTY);

    TestListMerge();
}

static void OsalTestSemList(void)
{
    OsalTestEventHandle *pos = NULL;
    OsalTestEventHandle *q = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(pos, q, &g_handleMng.list, OsalTestEventHandle, list) {
        if (pos->handle != NULL) {
            pos->handle(1);
        } else {
            HDF_LOGE("%s no handle\n", __func__);
            UT_TEST_CHECK_RET(true, OSAL_LIST_TRAVERSAL);
        }
    }
}

static bool g_noSemFlag = false;
static int ThreadTestPostSem(void *arg)
{
    static int cnt = 0;
    static int index = 0;
    HDF_STATUS ret;

    HDF_LOGI("[OSAL_UT_TEST]%s entry", __func__);
    (void)arg;

    while (g_threadTestFlag) {
        OsalMSleep(HDF_THREAD_TEST_SLEEP_MS);
        if (cnt % MSG_SEND_PERIOD == 0) {
            g_hdfSemStatus = true;
            g_hdfListStatus = true;
            index = 0;
            g_listCnt = 0;
            if (g_testEndFlag) {
                g_noSemFlag = true;
                break;
            }
            ret = OsalSemPost(&g_hdfSem);
            UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SEM_POST);
        }
        if (index == MSG_NOSEND_CNT) {
            UT_TEST_CHECK_RET(g_hdfSemStatus == true, OSAL_SEM_POST_RESULT);
            UT_TEST_CHECK_RET(g_hdfListStatus == true, OSAL_LIST_STRESS);
            UT_TEST_CHECK_RET(g_listCnt != g_listLen, OSAL_LIST_COUNT_CHECK);
            HDF_LOGE("g_listCnt %d %d", g_listCnt, g_listLen);
            g_hdfSemStatus = true;
            g_hdfListStatus = true;
        }

        if (index == MSG_NOSEND_CHECK_CNT) {
            UT_TEST_CHECK_RET(g_hdfSemStatus == false, OSAL_SEM_POST_RESULT);
            UT_TEST_CHECK_RET(g_hdfListStatus == false, OSAL_LIST_STRESS);
        }
#ifndef __USER__
        TestAddRoute(cnt);
#endif
        cnt++;
        index++;
    }
#ifndef __USER__
    OsalTestWorkEnd();
#endif
    OsalSemPost(&g_hdfSem);
    HDF_LOGE("%s exit", __func__);
    return 0;
}

static int ThreadTestWaitSem(void *arg)
{
    HDF_STATUS ret;

    HDF_LOGI("[OSAL_UT_TEST]%s entry", __func__);
    (void)arg;

    while (g_threadTestFlag) {
        ret = OsalSemWait(&g_hdfSem, HDF_WAIT_FOREVER);
        if (ret == HDF_SUCCESS) {
            OsalTestSemList();
            g_hdfSemStatus = false;
        } else {
            HDF_LOGE("[OSAL_UT_TEST]%s OsalSemWait fail", __func__);
            UT_TEST_CHECK_RET(true, OSAL_SEM_WAIT_FOREVER);
        }
        if (g_testEndFlag && g_noSemFlag) {
            break;
        }
    }
    HDF_LOGE("%s exit", __func__);
    return 0;
}

static void OsalOsalSemInterface(void)
{
    HDF_STATUS ret;
    struct OsalSem hdfSem = { NULL };

    ret = OsalSemInit(&hdfSem, 0);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SEM_CREATE);
    ret = OsalSemPost(&hdfSem);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SEM_POST);
    ret = OsalSemWait(&hdfSem, HDF_WAIT_FOREVER);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SEM_WAIT_FOREVER);
    ret = OsalSemWait(&hdfSem, 1);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_SEM_WAIT_TIMEOUT);
    ret = OsalSemDestroy(&hdfSem);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_SEM_DESTROY);

    HDF_LOGE("[OSAL_UT_TEST]%s no init test", __func__);
    ret = OsalSemPost(&hdfSem);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_SEM_VISIT_AFTER_DESTROY);
    ret = OsalSemWait(&hdfSem, HDF_WAIT_FOREVER);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_SEM_VISIT_AFTER_DESTROY);
    ret = OsalSemWait(&hdfSem, 1);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, OSAL_SEM_VISIT_AFTER_DESTROY);
    OsalSemDestroy(&hdfSem);
}

static void OsalTestSem(void)
{
    struct OsalThreadParam threadCfg;
    int32_t ret;

    HDF_LOGI("[OSAL_UT_TEST]%s start", __func__);

    OsalOsalSemInterface();

    UT_TEST_CHECK_RET(OsalSemInit(&g_hdfSem, 0) != HDF_SUCCESS, OSAL_SEM_CREATE);
    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name = "hdf_test_Post";
    threadCfg.priority = OSAL_THREAD_PRI_LOW;
    threadCfg.stackSize = HDF_TEST_STACK_SIZE;
    ret = OsalThreadCreate(&g_threadPostSem, (OsalThreadEntry)ThreadTestPostSem, (void *)&g_threadPara);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);

    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name = "hdf_test_Wait";
    threadCfg.priority = OSAL_THREAD_PRI_DEFAULT;
    threadCfg.stackSize = HDF_TEST_STACK_SIZE;
    ret = OsalThreadCreate(&g_threadWaitSem, (OsalThreadEntry)ThreadTestWaitSem, (void *)&g_threadPara);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);
    ret = OsalThreadStart(&g_threadWaitSem, &threadCfg);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);

    ret = OsalThreadStart(&g_threadPostSem, &threadCfg);
    UT_TEST_CHECK_RET(ret != HDF_SUCCESS, OSAL_THREAD_CREATE);
}

void OsalTestOther(int flag)
{
    HDF_LOGI("[OSAL_UT_TEST]%s entry flag:%d", __func__, flag);
#ifndef __USER__
    OsalTestWork(flag);
#endif
    OsalTestListInit();
    OsalTestSem();
}

void OsalStopThread(void)
{
    g_testEndFlag = true;
    OsalThreadDestroy(&g_threadPostSem);
    OsalThreadDestroy(&g_threadWaitSem);
}
