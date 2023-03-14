/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ioservstat_listener.h"
#include "osal_mem.h"

static int OnIoServiceEventReceive(
    struct HdfDevEventlistener *listener, struct HdfIoService *service, uint32_t id, struct HdfSBuf *data)
{
    if (listener == NULL || service == NULL || data == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    (void)id;
    struct ServiceStatus status = { 0 };
    if (ServiceStatusUnMarshalling(&status, data) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct IoServiceStatusListener *statusListener = listener->priv;
    if (statusListener->svcstatListener.callback != NULL &&
        (statusListener->deviceClass & status.deviceClass)) {
        statusListener->svcstatListener.callback(&statusListener->svcstatListener, &status);
    }

    return HDF_SUCCESS;
}

struct ServiceStatusListener *IoServiceStatusListenerNewInstance(void)
{
    struct IoServiceStatusListener *listener = OsalMemCalloc(sizeof(struct IoServiceStatusListener));
    if (listener == NULL) {
        return NULL;
    }

    listener->ioservListener.onReceive = OnIoServiceEventReceive;
    listener->ioservListener.priv = (void *)listener;

    return &listener->svcstatListener;
}

void IoServiceStatusListenerFree(struct ServiceStatusListener *listener)
{
    if (listener == NULL) {
        return;
    }
    struct IoServiceStatusListener *ioservListener
        = CONTAINER_OF(listener, struct IoServiceStatusListener, svcstatListener);
    OsalMemFree(ioservListener);
}