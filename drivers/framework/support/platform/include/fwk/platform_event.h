/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_EVENT_H
#define PLATFORM_EVENT_H

#include "hdf_base.h"
#include "hdf_dlist.h"
#include "osal_sem.h"
#include "osal_spinlock.h"
#include "osal_thread.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct PlatformEvent {
    OsalSpinlock spin;
    struct OsalSem sem;
    struct DListHead waiters;
    int32_t waiterCnt;
    uint32_t irqSave;
    uint32_t eventsWord;
};

enum PlatformEventType {
    PLAT_EVENT_DEVICE_DEAD = (0x1 << 0),
    PLAT_EVENT_RMSG_PENDING = (0x1 << 1),
    PLAT_EVENT_WMSG_PENDING = (0x1 << 2),
};

enum PlatformEventMode {
    PLAT_EVENT_MODE_AND     = 0x1 << 1,
    PLAT_EVENT_MODE_CLEAR   = 0x1 << 2,
    PLAT_EVENT_MODE_ASYNC   = 0x1 << 3,
};

struct PlatformEventListener {
    /** Private data passed to the callback function. */
    void *data;
    /** Mask bits for the interested events */
    uint32_t mask;
    /** The call back function for target events. */
    int32_t (*cb)(struct PlatformEventListener *listener, uint32_t events);
};

/**
 * @brief Initialize a platform event instance.
 *
 * @param pe Indicates the pointer to the the platform event instance
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PlatformEventInit(struct PlatformEvent *pe);

/**
 * @brief Uninitialize a platform event instance.
 *
 * @param pe Indicates the pointer to the the platform event instance
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PlatformEventUninit(struct PlatformEvent *pe);

/**
 * @brief Write the events to a platform event instance.
 *
 * @param pe Indicates the pointer to the the platform event instance
 * @param events The events to write.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PlatformEventPost(struct PlatformEvent *pe, uint32_t events);

/**
 * @brief Listen for CAN bus events.
 *
 * @param pe Indicates the pointer to the the platform event instance
 * @param mask Mask bits of the interested events.
 * @param mode Platorm event mode for this listening.
 * @param events Pointer for receiving the events.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PlatformEventWait(struct PlatformEvent *pe, uint32_t mask, int32_t mode, uint32_t tms, uint32_t *events);

/**
 * @brief Listen to a platform event instance.
 *
 * @param pe Indicates the pointer to the the platform event instance
 * @param listener The pointer to the listener.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t PlatformEventListen(struct PlatformEvent *pe, const struct PlatformEventListener *listener);

/**
 * @brief Unlisten to a platform event instance.
 *
 * @param pe Indicates the pointer to the the platform event instance
 * @param listener The pointer to the listener.
 *
 * @since 1.0
 */
void PlatformEventUnlisten(struct PlatformEvent *pe, const struct PlatformEventListener *listener);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLATFORM_EVENT_H */
