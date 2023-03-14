/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "dac_if.h"
#include "hdf_io_service_if.h"
#include "platform_core.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG dac_if_c
#define DAC_SERVICE_NAME "HDF_PLATFORM_DAC_MANAGER"

static void *DacManagerServiceGet(void)
{
    static struct HdfIoService *service = NULL;

    if (service != NULL) {
        return service;
    }
    service = (struct HdfIoService *)HdfIoServiceBind("HDF_PLATFORM_DAC_MANAGER");
    if (service == NULL) {
        HDF_LOGE("%s: failed to get dac manager service!", __func__);
    }
    return service;
}

DevHandle DacOpen(uint32_t number)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    uint32_t handle;

    service = DacManagerServiceGet();
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return NULL;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("DacOpen: malloc data failed!");
        return NULL;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("DacOpen: malloc reply failed!");
        HdfSbufRecycle(data);
        return NULL;
    }

    if (!HdfSbufWriteUint32(data, number)) {
        HDF_LOGE("DacOpen: write number failed!");
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return NULL;
    }

    ret = service->dispatcher->Dispatch(&service->object, DAC_IO_OPEN, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("DacOpen: service call open failed:%d", ret);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return NULL;
    }

    if (!HdfSbufReadUint32(reply, &handle)) {
        HDF_LOGE("DacOpen: read handle failed!");
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        return NULL;
    }
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return (DevHandle)(uintptr_t)handle;
}

void DacClose(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is invalid", __func__);
        return;
    }

    service = (struct HdfIoService *)DacManagerServiceGet();
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("DacClose: write handle failed!");
        HdfSbufRecycle(data);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, DAC_IO_CLOSE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("DacClose: close handle failed:%d", ret);
    }
    HdfSbufRecycle(data);
}

int32_t DacWrite(DevHandle handle, uint32_t channel, uint32_t val)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is invalid", __func__);
        return HDF_FAILURE;
    }

    service = (struct HdfIoService *)DacManagerServiceGet();
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("DacWrite: write handle failed!");
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteUint32(data, channel)) {
        HDF_LOGE("DacWrite: write channel failed!");
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteUint32(data, val)) {
        HDF_LOGE("DacWrite: write val failed!");
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, DAC_IO_WRITE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("DacWrite: write dac failed:%d", ret);
    }
    HdfSbufRecycle(data);
    return ret;
}
