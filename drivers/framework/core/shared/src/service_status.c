/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "hdf_service_status.h"

int ServiceStatusMarshalling(struct ServiceStatus *status, struct HdfSBuf *buf)
{
    if (status == NULL || buf == NULL || status->serviceName == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteString(buf, status->serviceName)
        || !HdfSbufWriteUint16(buf, status->deviceClass)
        || !HdfSbufWriteUint16(buf, status->status)
        || !HdfSbufWriteString(buf, status->info != NULL ? status->info : "")) {
        HDF_LOGI("failed to marshalling service status");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int ServiceStatusUnMarshalling(struct ServiceStatus *status, struct HdfSBuf *buf)
{
    if (status == NULL || buf == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    status->serviceName = HdfSbufReadString(buf);
    if (status->serviceName == NULL) {
        HDF_LOGI("failed to unmarshalling service status, service name is null");
        return HDF_FAILURE;
    }

    if (!HdfSbufReadUint16(buf, &status->deviceClass) || !HdfSbufReadUint16(buf, &status->status)) {
        HDF_LOGI("failed to unmarshalling service status, deviceClass or status invalid");
        return HDF_FAILURE;
    }

    status->info = HdfSbufReadString(buf);
    return HDF_SUCCESS;
}
