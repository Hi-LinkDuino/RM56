/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_SYSCALL_ADAPTER_H
#define HDF_SYSCALL_ADAPTER_H

#include <fcntl.h>
#include <poll.h>
#include <osal_mutex.h>
#include <osal_thread.h>
#include <hdf_dlist.h>
#include "hdf_io_service.h"

struct HdfSyscallAdapter;

enum HdfDevListenerThreadStatus {
    LISTENER_UNINITED = 0,
    LISTENER_INITED,
    LISTENER_EXITED,
    LISTENER_STARTED,
    LISTENER_RUNNING,
    LISTENER_WAITING,
};

struct HdfDevListenerThread {
    struct OsalMutex mutex;
    struct OsalThread thread;
    struct DListHead *adapterListPtr;
    struct HdfSyscallAdapter *adapter;
    struct pollfd *pfds;
    uint16_t pfdSize;
    bool pollChanged;
    bool shouldStop;
    struct DListHead *listenerListPtr;
    uint8_t status;
};

struct HdfSyscallAdapter {
    struct HdfIoService super;
    struct OsalMutex mutex;
    struct DListHead listenerList;
    int fd;
    struct DListHead listNode;
    struct HdfDevListenerThread *thread;
    struct HdfSyscallAdapterGroup *group;
};

struct HdfSyscallAdapterGroup {
    struct HdfIoServiceGroup serviceGroup;
    struct OsalMutex mutex;
    struct DListHead adapterList;
    struct HdfDevListenerThread *thread;
    struct DListHead listenerList;
};

#endif /* HDF_SYSCALL_ADAPTER_H */
