/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#ifndef HDF_SERVICE_STATUS_H
#define HDF_SERVICE_STATUS_H

#include "hdf_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfSBuf;
struct ServiceStatusListener;

enum ServiceStatusType {
    SERVIE_STATUS_START,
    SERVIE_STATUS_CHANGE,
    SERVIE_STATUS_STOP,
    SERVIE_STATUS_MAX,
};

enum ServiceStatusListenerCmd {
    SERVIE_STATUS_LISTENER_NOTIFY,
    SERVIE_STATUS_LISTENER_MAX,
};

struct ServiceStatus {
    const char* serviceName;
    uint16_t deviceClass;   /* ref to DeviceClass */
    uint16_t status;        /* ref to ServiceStatusType */
    const char *info;
};

typedef void (*OnServiceStatusReceived)(struct ServiceStatusListener *, struct ServiceStatus *);

struct ServiceStatusListener {
    OnServiceStatusReceived callback;
    void *priv;
};

int ServiceStatusMarshalling(struct ServiceStatus *status, struct HdfSBuf *buf);
int ServiceStatusUnMarshalling(struct ServiceStatus *status, struct HdfSBuf *buf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_SERVICE_STATUS_H */