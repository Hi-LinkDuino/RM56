/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#ifndef OSAL_SYSEVENT_H
#define OSAL_SYSEVENT_H

#include "hdf_dlist.h"

#define HDF_SYSEVENT 0xFADE

/* hdf sys event class definition */
#define HDF_SYSEVENT_CLASS_POWER  0x00000001
#define HDF_SYSEVENT_CLASS_MODULE 0x00000002

/* hdf power event definition */
enum PowerKeventId {
    KEVENT_POWER_SUSPEND,
    KEVENT_POWER_DISPLAY_OFF,
    KEVENT_POWER_RESUME,
    KEVENT_POWER_DISPLAY_ON,
    KEVENT_POWER_EVENT_MAX,
};

/* hdf power event definition */
enum DriverModuleKeventId {
    KEVENT_MODULE_INSTALL,
    KEVENT_MODULE_REMOVE,
    KEVENT_MODULE_EVENT_MAX,
};

struct HdfSysEvent {
    uint64_t eventClass;
    uint32_t eventid;
    const char *content;
    uint64_t syncToken;
};

struct HdfSysEventNotifyNode;

typedef int32_t (*HdfSysEventNotifierFn)(
    struct HdfSysEventNotifyNode *self, uint64_t eventClass, uint32_t event, const char *content);

struct HdfSysEventNotifyNode {
    HdfSysEventNotifierFn callback;
    struct DListHead listNode;
    uint64_t classFilter;
};

int32_t HdfSysEventNotifyRegister(struct HdfSysEventNotifyNode *notifierNode, uint64_t classSet);
void HdfSysEventNotifyUnregister(struct HdfSysEventNotifyNode *notifierNode);

__attribute__((weak)) int32_t HdfSysEventSend(uint64_t eventClass, uint32_t event, const char *content, bool sync);

#endif // #ifndef OSAL_SYSEVENT_H