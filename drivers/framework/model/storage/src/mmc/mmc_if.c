/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mmc_if.h"
#ifndef __USER__
#include "mmc_corex.h"
#endif
#ifdef __USER__
#include "hdf_io_service_if.h"
#endif
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG mmc_if_c

#define MMC_SVC_NAME_LEN 32

static void *MmcCntlrObjGetByNumber(int16_t id)
{
    void *object = NULL;
    char *serviceName = NULL;

    if (id < 0) {
        return NULL;
    }
    serviceName = OsalMemCalloc(MMC_SVC_NAME_LEN + 1);
    if (serviceName == NULL) {
        HDF_LOGE("MmcCntlrObjGetByNumber: OsalMemCalloc fail!");
        return NULL;
    }
    if (snprintf_s(serviceName, MMC_SVC_NAME_LEN + 1, MMC_SVC_NAME_LEN,
        "HDF_PLATFORM_MMC_%d", id) < 0) {
        HDF_LOGE("MmcCntlrObjGetByNumber: format service name fail!");
        OsalMemFree(serviceName);
        return object;
    }

#ifdef __USER__
    object = (void *)HdfIoServiceBind(serviceName);
#else
    object = (void *)MmcCntlrGetByNr(id);
#endif
    if (object == NULL) {
        HDF_LOGE("MmcCntlrObjGetByNumber: get service fail!");
    } else {
        HDF_LOGD("MmcCntlrObjGetByNumber: success");
    }

    OsalMemFree(serviceName);
    return object;
}

DevHandle MmcOpen(int16_t id)
{
    return (DevHandle)MmcCntlrObjGetByNumber(id);
}

void MmcClose(DevHandle handle)
{
    if (handle != NULL) {
#ifdef __USER__
        HdfIoServiceRecycle((struct HdfIoService *)handle);
#endif
    }
}

bool MmcDevPresent(DevHandle handle)
{
    if (handle == NULL) {
        return false;
    }
#ifdef __USER__
    int32_t ret;
    uint8_t present = 0;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = (struct HdfIoService *)handle;

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("MmcDevPresent: failed to obtain reply!");
        return false;
    }

    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("MmcDevPresent: dispatcher or Dispatch is NULL!");
        goto __EXIT;
    }
    ret = service->dispatcher->Dispatch(&service->object, MMC_CMD_DEV_PRESENT,
        NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("MmcDevPresent: failed to send service call:%d", ret);
        goto __EXIT;
    }

    if (HdfSbufReadUint8(reply, &present) == false) {
        HDF_LOGE("MmcDevPresent: read present fail!");
        goto __EXIT;
    }
__EXIT:
    HdfSbufRecycle(reply);
    return (present != 0);
#else
    return MmcCntlrDevPresent((struct MmcCntlr *)handle);
#endif
}
