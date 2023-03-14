/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "adc_if.h"

#define HDF_LOG_TAG adc_if_u_c
#define ADC_SERVICE_NAME "HDF_PLATFORM_ADC_MANAGER"

static void *AdcManagerGetService(void)
{
    static void *manager = NULL;

    if (manager != NULL) {
        return manager;
    }
    manager = (void *)HdfIoServiceBind(ADC_SERVICE_NAME);
    if (manager == NULL) {
        HDF_LOGE("%s: fail to get adc manager!", __func__);
    }
    return manager;
}

DevHandle AdcOpen(uint32_t number)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    uint32_t handle;

    service = (struct HdfIoService *)AdcManagerGetService();
    if (service == NULL) {
        return NULL;
    }
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: malloc data fail!", __func__);
        return NULL;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: malloc reply fail!", __func__);
        HdfSbufRecycle(data);
        return NULL;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)number)) {
        HDF_LOGE("%s: write number fail!", __func__);
        goto ERR;
    }

    ret = service->dispatcher->Dispatch(&service->object, ADC_IO_OPEN, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: service call open fail:%d", __func__, ret);
        goto ERR;
    }

    if (!HdfSbufReadUint32(reply, &handle)) {
        HDF_LOGE("%s: read handle fail!", __func__);
        goto ERR;
    }
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return (DevHandle)(uintptr_t)handle;
ERR:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return NULL;
}

void AdcClose(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *data = NULL;

    service = (struct HdfIoService *)AdcManagerGetService();
    if (service == NULL) {
        return;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        return;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%s: write handle fail!", __func__);
        HdfSbufRecycle(data);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, ADC_IO_CLOSE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: close handle fail:%d", __func__, ret);
    }
    HdfSbufRecycle(data);
}

int32_t AdcRead(DevHandle handle, uint32_t channel, uint32_t *val)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
	
    service = (struct HdfIoService *)AdcManagerGetService();
    if (service == NULL) {
        return HDF_PAL_ERR_DEV_CREATE;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%s: write handle fail!", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)channel)) {
        HDF_LOGE("%s: write adc number failed!", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }
    ret = service->dispatcher->Dispatch(&service->object, ADC_IO_READ, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to send service call:%d", __func__, ret);
        goto EXIT;
    }

    if (!HdfSbufReadUint32(reply, val)) {
        HDF_LOGE("%s: read sbuf failed", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    goto EXIT;
EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}
