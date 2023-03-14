/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "hdf_log.h"
#include "hdf_message_test.h"
#include "hdf_wlan_priority_queue.h"
#include "osal_thread.h"
#include "osal_time.h"


#define TEST_QUEUE_SIZE 30

#define NO_PRIORITY 1
#define MUTI_PRIORITY 2

#define POP_TIMEOUT 3000
int32_t MessageQueueTest001(void)
{
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    int32_t errCode;
    void *p = NULL;
    PriorityQueue *queue = CreatePriorityQueue(TEST_QUEUE_SIZE, NO_PRIORITY);
    if (queue == NULL) {
        HDF_LOGE("%s:Create queue failed!", __func__);
        return -1;
    }
    do {
        errCode = PushPriorityQueue(queue, 0, &a);
        MSG_BREAK_IF_NOT_SUCCESS(errCode);

        errCode = PushPriorityQueue(queue, 0, &b);
        MSG_BREAK_IF_NOT_SUCCESS(errCode);

        errCode = PushPriorityQueue(queue, 0, &c);
        MSG_BREAK_IF_NOT_SUCCESS(errCode);

        errCode = PushPriorityQueue(queue, 0, &d);
        MSG_BREAK_IF_NOT_SUCCESS(errCode);

        p = PopPriorityQueue(queue, 0);
        MSG_BREAK_IF(errCode, p == NULL);
        MSG_BREAK_IF(errCode, p != &a);

        p = PopPriorityQueue(queue, 0);
        MSG_BREAK_IF(errCode, p == NULL);
        MSG_BREAK_IF(errCode, p != &b);

        p = PopPriorityQueue(queue, 0);
        MSG_BREAK_IF(errCode, p == NULL);
        MSG_BREAK_IF(errCode, p != &c);

        p = PopPriorityQueue(queue, 0);
        MSG_BREAK_IF(errCode, p == NULL);
        MSG_BREAK_IF(errCode, p != &d);
    } while (0);

    DestroyPriorityQueue(queue);

    return errCode;
}

// Proprity test
int32_t MessageQueueTest002(void)
{
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    int32_t errCode;
    void *p = NULL;
    PriorityQueue *queue = NULL;
    queue = CreatePriorityQueue(TEST_QUEUE_SIZE, MUTI_PRIORITY);
    if (queue == NULL) {
        HDF_LOGE("%s:Create queue failed!", __func__);
        return -1;
    }
    do {
        errCode = PushPriorityQueue(queue, 0, &a);
        MSG_BREAK_IF_NOT_SUCCESS(errCode);

        errCode = PushPriorityQueue(queue, 1, &b);
        MSG_BREAK_IF_NOT_SUCCESS(errCode);

        errCode = PushPriorityQueue(queue, 1, &c);
        MSG_BREAK_IF_NOT_SUCCESS(errCode);

        errCode = PushPriorityQueue(queue, 0, &d);
        MSG_BREAK_IF_NOT_SUCCESS(errCode);

        p = PopPriorityQueue(queue, 0);
        MSG_BREAK_IF(errCode, p == NULL);
        MSG_BREAK_IF(errCode, p != &a);

        p = PopPriorityQueue(queue, 0);
        MSG_BREAK_IF(errCode, p == NULL);
        MSG_BREAK_IF(errCode, p != &d);

        p = PopPriorityQueue(queue, 0);
        MSG_BREAK_IF(errCode, p == NULL);
        MSG_BREAK_IF(errCode, p != &b);

        p = PopPriorityQueue(queue, 0);
        MSG_BREAK_IF(errCode, p == NULL);
        MSG_BREAK_IF(errCode, p != &c);
    } while (0);

    DestroyPriorityQueue(queue);
    return errCode;
}

int g_testValue = 0;
#define PUSH_DELAY 2000
static int RunPushQueue(void *para)
{
    if (para == NULL) {
        return -1;
    }
    OsalMSleep(PUSH_DELAY);
    PushPriorityQueue((PriorityQueue *)para, 0, &g_testValue);
    return 0;
}

// wait and awake test
int32_t MessageQueueTest003(void)
{
    int32_t errCode = HDF_SUCCESS;
    void *p = NULL;
    PriorityQueue *queue = NULL;
    int32_t status;
    struct OsalThreadParam config;
    OSAL_DECLARE_THREAD(pushThread);
    queue = CreatePriorityQueue(TEST_QUEUE_SIZE, NO_PRIORITY);
    if (queue == NULL) {
        HDF_LOGE("%s:Create queue failed!", __func__);
        return -1;
    }

    do {
        config.name = "PushQueueWithDelay";
        config.priority = OSAL_THREAD_PRI_DEFAULT;
        config.stackSize = 0x1000;
        status = OsalThreadCreate(&pushThread, RunPushQueue, queue);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("%s:OsalThreadCreate failed!status=%d", __func__, status);
            errCode = HDF_FAILURE;
            break;
        }
        status = OsalThreadStart(&pushThread, &config);
        if (status != HDF_SUCCESS) {
            HDF_LOGE("%s:OsalThreadStart failed!status=%d", __func__, status);
            OsalThreadDestroy(&pushThread);
            errCode = HDF_FAILURE;
            break;
        }

        p = PopPriorityQueue(queue, POP_TIMEOUT);
        MSG_BREAK_IF(errCode, p == NULL);
        MSG_BREAK_IF(errCode, p != &g_testValue);
    } while (false);

    OsalThreadDestroy(&pushThread);
    DestroyPriorityQueue(queue);
    return errCode;
}
