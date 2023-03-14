/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "platform_queue_test.h"
#include "platform_assert.h"
#include "platform_queue.h"

#define HDF_LOG_TAG platform_queue_test

#define PLAT_QUEUE_TEST_TIMEOUT 20

#define TEST_CODE_A 0x5A

struct PlatformQueueTestMsg {
    struct PlatformMsg msg;
    struct OsalSem sem;
    int32_t status;
};

static int32_t PlatformQueueTestHandle(struct PlatformQueue *queue, struct PlatformMsg *msg)
{
    struct PlatformQueueTestMsg *tmsg = (struct PlatformQueueTestMsg *)msg;
    if (msg == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (queue == NULL) {
        tmsg->status = HDF_ERR_INVALID_OBJECT;
        PLAT_LOGE("%s: queue object is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (msg->data != queue) {
        tmsg->status = HDF_ERR_INVALID_OBJECT;
        PLAT_LOGE("%s: queue object is wrong", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (msg->code != TEST_CODE_A) {
        tmsg->status = HDF_ERR_INVALID_PARAM;
        PLAT_LOGE("%s: code is wrong:%d", __func__, msg->code);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)OsalSemPost(&tmsg->sem);
    tmsg->status = HDF_SUCCESS;
    return HDF_SUCCESS;
}

static int32_t PlatformQueueTestAddAndWait(struct PlatformQueue *pq)
{
    int32_t ret;
    struct PlatformQueueTestMsg tmsg;

    PLAT_LOGD("%s: enter", __func__);
    tmsg.msg.code = TEST_CODE_A;
    tmsg.msg.data = pq;
    tmsg.status = -1;

    (void)OsalSemInit(&tmsg.sem, 0);
    // should add msg success
    ret = PlatformQueueAddMsg(pq, &tmsg.msg);
    if (!CHECK_EQ(ret, HDF_SUCCESS)) {
        (void)OsalSemDestroy(&tmsg.sem);
        return ret;
    }

    // should wait msg success
    ret = OsalSemWait(&tmsg.sem, PLAT_QUEUE_TEST_TIMEOUT);
    if (!CHECK_EQ(ret, HDF_SUCCESS)) {
        (void)OsalSemDestroy(&tmsg.sem);
        return ret;
    }
    if (!CHECK_EQ(tmsg.status, HDF_SUCCESS)) {
        (void)OsalSemDestroy(&tmsg.sem);
        return tmsg.status;
    }

    (void)OsalSemDestroy(&tmsg.sem);
    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformQueueTestReliability(struct PlatformQueue *pq)
{
    int32_t ret;
    struct PlatformMsg msg;

    PLAT_LOGD("%s: enter", __func__);
    ret = PlatformQueueAddMsg(NULL, &msg);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, ret);

    ret = PlatformQueueAddMsg(pq, NULL);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, ret);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

struct PlatformQueueTestEntry {
    int cmd;
    int32_t (*func)(struct PlatformQueue *pq);
    const char *name;
};

static struct PlatformQueueTestEntry g_entry[] = {
    { PLAT_QUEUE_TEST_ADD_AND_WAIT, PlatformQueueTestAddAndWait, "PlatformQueueTestAddAndWait" },
    { PLAT_QUEUE_TEST_RELIABILITY, PlatformQueueTestReliability, "PlatformQueueTestReliability" },
};

int PlatformQueueTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;
    struct PlatformQueue *pq = NULL;
    struct PlatformQueueTestEntry *entry = NULL;

    if (cmd > PLAT_QUEUE_TEST_CMD_MAX) {
        PLAT_LOGE("PlatformQueueTestExecute: invalid cmd:%d", cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        PLAT_LOGE("[PlatformQueueTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
        return ret;
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        entry = &g_entry[i];
        break;
    }

    if (entry == NULL) {
        PLAT_LOGE("%s: no entry matched, cmd = %d", __func__, cmd);
        return HDF_ERR_NOT_SUPPORT;
    }

    pq = PlatformQueueCreate(PlatformQueueTestHandle, "platform_queue_test", NULL);
    if (pq == NULL) {
        PLAT_LOGE("%s: create queue failed", __func__);
        return HDF_FAILURE;
    }

    ret = PlatformQueueStart(pq);
    if (ret != HDF_SUCCESS) {
        PLAT_LOGE("%s: start queue failed, ret = %d", __func__, ret);
        PlatformQueueDestroy(pq);
        return ret;
    }

    ret = entry->func(pq);
    PlatformQueueDestroy(pq);

    PLAT_LOGE("[PlatformQueueTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
    return ret;
}

void PlatformQueueTestExecuteAll(void)
{
    int32_t i;
    int32_t ret;
    int32_t fails = 0;

    for (i = 0; i < PLAT_QUEUE_TEST_CMD_MAX; i++) {
        ret = PlatformQueueTestExecute(i);
        fails += (ret != HDF_SUCCESS) ? 1 : 0;
    }

    PLAT_LOGE("PlatformQueueTestExecuteALL: **********PASS:%d  FAIL:%d************\n\n",
        PLAT_QUEUE_TEST_CMD_MAX - fails, fails);
}
