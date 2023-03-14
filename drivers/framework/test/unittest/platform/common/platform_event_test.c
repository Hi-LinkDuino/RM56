/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "platform_event_test.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_sem.h"
#include "osal_spinlock.h"
#include "osal_time.h"
#include "platform_assert.h"
#include "platform_event.h"

#define HDF_LOG_TAG platform_event_test

#define PLAT_EVENT_TEST_TIMEOUT 10

enum PlatformTestEvent {
    PLAT_TEST_EVENT_A = 0x1,
    PLAT_TEST_EVENT_B = 0x2,
    PLAT_TEST_EVENT_C = 0x4,
};

static int32_t PlatformEventTestInitAndUninit(struct PlatformEvent *pe)
{
    int32_t ret;

    PLAT_LOGD("%s: enter", __func__);
    // eventsWord should be 0 after init
    CHECK_EQ_RETURN(pe->eventsWord, 0, HDF_FAILURE);

    // can use spinlock after init
    ret = OsalSpinLock(&pe->spin);
    if (!CHECK_EQ(ret, HDF_SUCCESS)) {
        return HDF_FAILURE;
    }
    (void)OsalSpinUnlock(&pe->spin);

    // can use sem after init
    ret = OsalSemPost(&pe->sem);
    if (!CHECK_EQ(ret, HDF_SUCCESS)) {
        return HDF_FAILURE;
    }
    (void)OsalSemWait(&pe->sem, HDF_WAIT_FOREVER);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformEventTestPostAndWait(struct PlatformEvent *pe)
{
    int32_t ret;
    uint32_t events;

    PLAT_LOGD("%s: enter", __func__);
    // should wait timeout before post
    ret = PlatformEventWait(pe, PLAT_TEST_EVENT_A, 0, PLAT_EVENT_TEST_TIMEOUT, &events);
    CHECK_EQ_RETURN(ret, HDF_ERR_TIMEOUT, HDF_FAILURE);

    // should post success
    ret = PlatformEventPost(pe, PLAT_TEST_EVENT_A);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    // should wait success after post
    ret = PlatformEventWait(pe, PLAT_TEST_EVENT_A, 0, PLAT_EVENT_TEST_TIMEOUT, &events);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);
    CHECK_EQ_RETURN(events, PLAT_TEST_EVENT_A, HDF_FAILURE);

    // should wait timeut if wait again
    ret = PlatformEventWait(pe, PLAT_TEST_EVENT_A, 0, PLAT_EVENT_TEST_TIMEOUT, &events);
    CHECK_EQ_RETURN(ret, HDF_ERR_TIMEOUT, HDF_FAILURE);

    // should post success for multi events
    ret = PlatformEventPost(pe, PLAT_TEST_EVENT_A | PLAT_TEST_EVENT_B);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);
    
    // should wait timeout if events not match
    ret = PlatformEventWait(pe, PLAT_TEST_EVENT_B | PLAT_TEST_EVENT_C, PLAT_EVENT_MODE_AND,
        PLAT_EVENT_TEST_TIMEOUT, &events);
    CHECK_EQ_RETURN(ret, HDF_ERR_TIMEOUT, HDF_FAILURE);
    ret = PlatformEventWait(pe, PLAT_TEST_EVENT_C, 0, PLAT_EVENT_TEST_TIMEOUT, &events);
    CHECK_EQ_RETURN(ret, HDF_ERR_TIMEOUT, HDF_FAILURE);

    // should wait success if events match
    ret = PlatformEventWait(pe, PLAT_TEST_EVENT_A | PLAT_TEST_EVENT_B, PLAT_EVENT_MODE_AND,
        PLAT_EVENT_TEST_TIMEOUT, &events);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);
    CHECK_EQ_RETURN(events, PLAT_TEST_EVENT_A | PLAT_TEST_EVENT_B, HDF_FAILURE);

    // should wait timeut if wait again
    ret = PlatformEventWait(pe, PLAT_TEST_EVENT_A, 0, PLAT_EVENT_TEST_TIMEOUT, &events);
    CHECK_EQ_RETURN(ret, HDF_ERR_TIMEOUT, HDF_FAILURE);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformEventListenTestCb(struct PlatformEventListener *listener, uint32_t events)
{
    uint32_t *pEvents = (uint32_t *)listener->data;

    *pEvents = events;
    return HDF_SUCCESS;
}

static int32_t PlatformEventTestListenAndUnliten(struct PlatformEvent *pe)
{
    int32_t ret;
    struct PlatformEventListener listener;
    uint32_t events = 0;

    PLAT_LOGD("%s: enter", __func__);
    listener.mask = PLAT_TEST_EVENT_A | PLAT_TEST_EVENT_B;
    listener.cb = PlatformEventListenTestCb;
    listener.data = (void *)&events;

    // should listen success
    ret = PlatformEventListen(pe, &listener);
    CHECK_EQ_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    // no events got before post
    OsalMSleep(PLAT_EVENT_TEST_TIMEOUT);
    CHECK_EQ_RETURN(events, 0, HDF_FAILURE);

    PlatformEventPost(pe, PLAT_TEST_EVENT_A);
    OsalMSleep(PLAT_EVENT_TEST_TIMEOUT);
    // should got the events in callback
    CHECK_EQ_RETURN(events, PLAT_TEST_EVENT_A, HDF_FAILURE);

    events = 0;
    PlatformEventPost(pe, PLAT_TEST_EVENT_B);
    OsalMSleep(PLAT_EVENT_TEST_TIMEOUT);
    // should got the events in callback
    CHECK_EQ_RETURN(events, PLAT_TEST_EVENT_B, HDF_FAILURE);

    events = 0;
    PlatformEventPost(pe, PLAT_TEST_EVENT_A | PLAT_TEST_EVENT_B);
    OsalMSleep(PLAT_EVENT_TEST_TIMEOUT);
    // should got the events in callback
    CHECK_EQ_RETURN(events, PLAT_TEST_EVENT_A | PLAT_TEST_EVENT_B, HDF_FAILURE);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

static int32_t PlatformEventTestReliability(struct PlatformEvent *pe)
{
    int32_t ret;
    uint32_t events;
    struct PlatformEventListener listener;

    PLAT_LOGD("%s: enter", __func__);
    // should return invalid obj when init null pointer
    ret = PlatformEventInit(NULL);
    CHECK_EQ_RETURN(ret, HDF_ERR_INVALID_OBJECT, HDF_FAILURE);

    // should not wait success when event instance is NULL
    ret = PlatformEventWait(NULL, 0x1, 0, PLAT_EVENT_TEST_TIMEOUT, &events);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    // should not wait success when events is NULL
    ret = PlatformEventWait(pe, 0x1, 0, PLAT_EVENT_TEST_TIMEOUT, NULL);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    // should not wait success when mask is 0
    ret = PlatformEventWait(pe, 0, 0, PLAT_EVENT_TEST_TIMEOUT, &events);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    // should not listen success when event instance is NULL
    ret = PlatformEventListen(NULL, &listener);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    // should not wait success when listener is NULL
    ret = PlatformEventListen(pe, NULL);
    CHECK_NE_RETURN(ret, HDF_SUCCESS, HDF_FAILURE);

    PLAT_LOGD("%s: exit", __func__);
    return HDF_SUCCESS;
}

struct PlatformEventTestEntry {
    int cmd;
    int32_t (*func)(struct PlatformEvent *pe);
    const char *name;
};

static struct PlatformEventTestEntry g_entry[] = {
    { PLAT_EVENT_TEST_INIT_AND_UNINIT, PlatformEventTestInitAndUninit, "PlatformEventTestInitAndUninit" },
    { PLAT_EVENT_TEST_POST_AND_WAIT, PlatformEventTestPostAndWait, "PlatformEventTestPostAndWait" },
    { PLAT_EVENT_TEST_LISTEN_AND_UNLISTEN, PlatformEventTestListenAndUnliten, "PlatformEventTestListenAndUnliten" },
    { PLAT_EVENT_TEST_RELIABILITY, PlatformEventTestReliability, "PlatformEventTestReliability" },
};

int PlatformEventTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;
    struct PlatformEvent pe;
    struct PlatformEventTestEntry *entry = NULL;

    if (cmd > PLAT_EVENT_TEST_CMD_MAX) {
        PLAT_LOGE("PlatformEventTestExecute: invalid cmd:%d", cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        PLAT_LOGE("[PlatformEventTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
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

    if ((ret = PlatformEventInit(&pe)) != HDF_SUCCESS) {
        PLAT_LOGE("%s: init failed, ret = %d", __func__, ret);
        return ret;
    }

    ret = entry->func(&pe);
    PlatformEventUninit(&pe);

    PLAT_LOGE("[PlatformEventTestExecute][======cmd:%d====ret:%d======]", cmd, ret);
    return ret;
}

void PlatformEventTestExecuteAll(void)
{
    int32_t i;
    int32_t ret;
    int32_t fails = 0;

    for (i = 0; i < PLAT_EVENT_TEST_CMD_MAX; i++) {
        ret = PlatformEventTestExecute(i);
        fails += (ret != HDF_SUCCESS) ? 1 : 0;
    }

    PLAT_LOGE("PlatformEventTestExecuteALL: **********PASS:%d  FAIL:%d************\n\n",
        PLAT_EVENT_TEST_CMD_MAX - fails, fails);
}
