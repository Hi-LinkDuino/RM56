/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "platform_event.h"
#include "hdf_dlist.h"
#include "osal_mem.h"
#include "osal_sem.h"
#include "platform_errno.h"
#include "platform_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct PlatformEventWaiter {
    struct DListHead node;
    struct OsalSem sem;
    uint32_t mask;
    uint32_t mode;
    void *data;
    int32_t (*cb)(struct PlatformEventWaiter *waiter, int32_t events);
};

int32_t PlatformEventInit(struct PlatformEvent *pe)
{
    if (pe == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    pe->eventsWord = 0;
    pe->waiterCnt = 0;
    DListHeadInit(&pe->waiters);
    (void)OsalSpinInit(&pe->spin);
    (void)OsalSemInit(&pe->sem, 0);
    return HDF_SUCCESS;
}

static void PlatformEventLock(struct PlatformEvent *pe)
{
    (void)OsalSpinLockIrqSave(&pe->spin, &pe->irqSave);
}

static void PlatformEventUnlock(struct PlatformEvent *pe)
{
    (void)OsalSpinUnlockIrqRestore(&pe->spin, &pe->irqSave);
}

static void PlatformEventClearWaiters(struct PlatformEvent *pe)
{
    struct PlatformEventWaiter *waiter = NULL;
    struct PlatformEventWaiter *tmp = NULL;

    PlatformEventLock(pe);
    DLIST_FOR_EACH_ENTRY_SAFE(waiter, tmp, &pe->waiters, struct PlatformEventWaiter, node) {
        DListRemove(&waiter->node);
        if ((waiter->mode & PLAT_EVENT_MODE_ASYNC) != 0) {
            OsalMemFree(waiter);
        }
    }
    PlatformEventUnlock(pe);
}

int32_t PlatformEventUninit(struct PlatformEvent *pe)
{
    if (pe == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    PlatformEventClearWaiters(pe);
    (void)OsalSemDestroy(&pe->sem);
    (void)OsalSpinDestroy(&pe->spin);
    return HDF_SUCCESS;
}

int32_t PlatformEventPost(struct PlatformEvent *pe, uint32_t events)
{
    int32_t ret;
    uint32_t masked;
    struct PlatformEventWaiter *waiter = NULL;
    struct PlatformEventWaiter *pos = NULL;
    struct PlatformEventWaiter *tmp = NULL;

    if (pe == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (events == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    PlatformEventLock(pe);
    pe->eventsWord |= events;
    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &pe->waiters, struct PlatformEventWaiter, node) {
        if (((pos->mode & PLAT_EVENT_MODE_AND) == 0) &&
            ((pe->eventsWord & pos->mask) != 0)) {
            waiter = pos;
            break;
        }
        if (((pos->mode & PLAT_EVENT_MODE_AND) != 0) &&
            ((pe->eventsWord & pos->mask) == pos->mask)) {
            waiter = pos;
            break;
        }
    }
    if (waiter != NULL) {
        masked = pe->eventsWord & waiter->mask;
        if ((waiter->mode & PLAT_EVENT_MODE_ASYNC) != 0) {
            pe->eventsWord &= ~masked;
        } else {
            DListRemove(&waiter->node);
        }
    }
    PlatformEventUnlock(pe);

    if (waiter == NULL) {
        return HDF_SUCCESS;
    }

    if ((waiter->mode & PLAT_EVENT_MODE_ASYNC) == 0) {
        ret = OsalSemPost(&waiter->sem);
    } else if (waiter->cb != NULL) {
        ret = waiter->cb(waiter, masked);
    } else {
        ret = HDF_ERR_INVALID_PARAM;
    }

    return ret;
}

static int32_t PlatformEventRead(struct PlatformEvent *pe, uint32_t *events, uint32_t mask, int32_t mode)
{
    uint32_t masked = 0;

    if (events == NULL || mask == 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    PlatformEventLock(pe);
    if (((uint32_t)mode & (uint32_t)PLAT_EVENT_MODE_AND) == 0) {
        if ((pe->eventsWord & mask) != 0) {
            masked = pe->eventsWord & mask;
        }
    } else {
        if ((pe->eventsWord & mask) == mask) {
            masked = pe->eventsWord & mask;
        }
    }
    pe->eventsWord &= ~masked; // clear the bits default
    PlatformEventUnlock(pe);

    *events = masked;
    return HDF_SUCCESS;
}

static int32_t PlatformEventReadSlowlly(struct PlatformEvent *pe,
    uint32_t *events, uint32_t mask, int32_t mode, uint32_t tms)
{
    int32_t ret;
    struct PlatformEventWaiter waiter;

    waiter.mask = mask;
    waiter.mode = (uint32_t)mode & (uint32_t)(~PLAT_EVENT_MODE_ASYNC);
    (void)OsalSemInit(&waiter.sem, 0);
    DListHeadInit(&waiter.node);

    PlatformEventLock(pe);
    DListInsertTail(&waiter.node, &pe->waiters);
    PlatformEventUnlock(pe);

    ret = OsalSemWait(&waiter.sem, tms);
    if (ret == HDF_SUCCESS) {
        (void)OsalSemDestroy(&waiter.sem);
        return PlatformEventRead(pe, events, mask, mode);
    }

    PlatformEventLock(pe);
    // the waiter may be still in the list, we need to check...
    if (waiter.node.next != NULL && !DListIsEmpty(&waiter.node)) {
        DListRemove(&waiter.node);
    }
    PlatformEventUnlock(pe);

    (void)OsalSemDestroy(&waiter.sem);
    return ret;
}

int32_t PlatformEventWait(struct PlatformEvent *pe, uint32_t mask, int32_t mode, uint32_t tms, uint32_t *events)
{
    int32_t ret;
    uint32_t eventsRead;

    if (pe == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (events == NULL || mask == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PlatformEventRead(pe, &eventsRead, mask, mode);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (eventsRead != 0) {
        *events = eventsRead;
        return HDF_SUCCESS;
    }

    if (tms == 0) {
        return HDF_PLT_ERR_NO_DATA;
    }

    return PlatformEventReadSlowlly(pe, events, mask, mode, tms);
}

static int32_t PlatformEventAsyncCallback(struct PlatformEventWaiter *waiter, int32_t events)
{
    struct PlatformEventListener *listener = (struct PlatformEventListener *)waiter->data;

    if (listener == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    // gona do it in thread context later ...
    return listener->cb(listener, (uint32_t)events & listener->mask);
}

int32_t PlatformEventListen(struct PlatformEvent *pe, const struct PlatformEventListener *listener)
{
    struct PlatformEventWaiter *waiter = NULL;

    if (pe == NULL || listener == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (listener->mask == 0 || listener->cb == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    waiter = (struct PlatformEventWaiter *)OsalMemCalloc(sizeof(*waiter));
    if (waiter == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    waiter->mask = listener->mask;
    waiter->data = (void *)listener;
    waiter->mode = PLAT_EVENT_MODE_ASYNC;
    waiter->cb = PlatformEventAsyncCallback;

    PlatformEventLock(pe);
    DListHeadInit(&waiter->node);
    DListInsertTail(&waiter->node, &pe->waiters);
    PlatformEventUnlock(pe);

    return HDF_SUCCESS;
}

void PlatformEventUnlisten(struct PlatformEvent *pe, const struct PlatformEventListener *listener)
{
    struct PlatformEventWaiter *waiter = NULL;
    struct PlatformEventWaiter *tmp = NULL;

    if (pe == NULL || listener == NULL) {
        return;
    }

    PlatformEventLock(pe);
    DLIST_FOR_EACH_ENTRY_SAFE(waiter, tmp, &pe->waiters, struct PlatformEventWaiter, node) {
        if ((waiter->mode & PLAT_EVENT_MODE_ASYNC) == 0) {
            continue;
        }
        if (waiter->data == (void *)listener) {
            DListRemove(&waiter->node);
            OsalMemFree(waiter);
            break;
        }
    }
    PlatformEventUnlock(pe);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
