/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_VNODE_ADAPTER_H
#define HDF_VNODE_ADAPTER_H
#include "hdf_dlist.h"
#include "osal_mutex.h"
#include "hdf_sbuf.h"
#include "hdf_io_service.h"

struct OsalCdev;

struct HdfVNodeAdapter {
    struct HdfIoService ioService;
    char *vNodePath;
    struct OsalMutex mutex;
    struct DListHead clientList;
    struct OsalCdev *cdev;
};

struct HdfDevEvent {
    uint32_t id;
    struct HdfSBuf *data;
    struct DListHead listNode;
};

#endif /* HDF_VNODE_ADAPTER_H */

