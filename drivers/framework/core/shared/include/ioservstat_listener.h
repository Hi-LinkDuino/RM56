/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_IOSERVICE_STATUS_LISTENER_H
#define HDF_IOSERVICE_STATUS_LISTENER_H

#include "hdf_dlist.h"
#include "hdf_io_service_if.h"
#include "hdf_service_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct IoServiceStatusListener {
    struct ServiceStatusListener svcstatListener;
    struct HdfDevEventlistener ioservListener;
    struct DListHead node;
    uint16_t deviceClass;
};

struct ServiceStatusListener *IoServiceStatusListenerNewInstance(void);
void IoServiceStatusListenerFree(struct ServiceStatusListener *listener);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // HDF_IOSERVICE_STATUS_LISTENER_H
